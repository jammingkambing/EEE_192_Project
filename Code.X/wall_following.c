/*
 * ---------------------------------------------------------------------------
 * DARC BOT - WALL FOLLOWING SUBSYSTEM (SENSOR + DECISION + PID)
 * Author: Kathleen Patajo
 * ---------------------------------------------------------------------------
 * 
 * WHAT THIS FILE DOES:
 *   - Reads 3 ultrasonic sensors (Right, Front, Left)
 *   - Converts raw sensor values to centimeters
 *   - Uses RHR
 *   - Uses PID math
 * 
 * OUTPUTS:
 *   - action_code    = 0=FORWARD, 1=TURN RIGHT, 2=TURN LEFT, 3=TURN AROUND
 *   - pid_correction = -12 to +12 (negative = turn LEFT, positive = turn RIGHT)
 * 
 * SENSOR READINGS (for debugging):
 *   - right_cm, front_cm, left_cm = distances in cm
 * 
 * SENSOR PINOUT-----------------
 * 
 *   RIGHT sensor:  PA17 (TRIG), PA18 (ECHO) - PORT A GROUP 0
 *   FRONT sensor:  PA00 (TRIG), PA01 (ECHO) - PORT A GROUP 0
 *   LEFT sensor:   PB02 (TRIG), PB22 (ECHO) - PORT B GROUP 1
 * 
 * RIGHT-HAND RULE LOGIC (Priority: Right)------------------
 * 
 *   1. Right side OPEN?      ? TURN RIGHT (find the wall)
 *   2. Front OPEN?           ? FORWARD (follow wall)
 *   3. Left OPEN?            ? TURN LEFT (dead end avoidance)
 *   4. All BLOCKED?          ? TURN AROUND (dead end)
 * 
 * ---------------------------------------------------
 * 
 * standalone test 
 * 1. use temporary motor functions (placeholders)
 * 2. test logic via PuTTY
 * 3. no motors
 * 
 * when integrating to GITHUB
 * 1. replace temporary motor functions with #include "locomotion.h"
 * 2. remove temporary motor functions
 * 3. robot moves
 * 
 * 
 */

#include "platform/init.h"
#include "platform/usart.h"
#include <xc.h>
#include <string.h>
#include <stdio.h>


// GIITHUB INTEGRATION: uncomment when on github
//#include "locomotion.h
// for go_forward(), turn_right(), turn_left(), set_a_speed(), set_b_speed()


// ------------------------ THRESHOLDS ------------------------
#define SIDE_THRESHOLD_CM     20   // >20cm = no wall, turn right
#define FRONT_THRESHOLD_CM    12   // <12cm = obstacle ahead

// ------------------------ PID SETTINGS ------------------------
#define DESIRED_WALL_DIST_CM  8    // How close to right wall (cm)
#define KP_WALL               1.5  // How hard to steer
#define KD_WALL               0.1  // Smooth out wobbles
#define KI_WALL               0.01 // Fix small steady errors

// ------------------------ MOTOR SPEEDS ------------------------
#define BASE_SPEED            50   // Normal forward speed
#define MAX_SPEED             80   // Speed limit
#define MIN_SPEED             20   // Minimum speed
#define TURN_SPEED            60   // Turning speed

// ------------------------ PID MEMORY ------------------------
static float integral = 0;
static float previous_error = 0;
static uint32_t last_pid_time = 0;

// ------------------------ BANNER ------------------------
static const char banner_msg[] =
"\033[2J\033[1;1H"
"+----------------------------------------------------------------+\r\n"
"| EEE 192: WALL-FOLLOWING USING HC-SR04P SENSORS                 |\r\n"
"| RIGHT, FRONT & LEFT SENSOR TEST                                |\r\n"
"+----------------------------------------------------------------+\r\n"
"\r\n";

// ------------------------ DELAYS ------------------------
static void delay_us(uint32_t us) {
    for (volatile uint32_t i = 0; i < (us * 5); i++);  // rough 1us per 5 loops at 24MHz
}

static void delay_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < (ms * 5000); i++); // rough 1ms per 5000 loops
}

// ------------------------ READ RIGHT SENSOR ------------------------
// PA17 (TRIG), PA18 (ECHO) - PORT A GROUP 0
uint32_t read_right_sensor_raw(void) {
    uint32_t echo_time = 0;
    uint32_t timeout;
    
    PORT_SEC_REGS->GROUP[0].PORT_OUTCLR = (1 << 17);  // TRIG low
    delay_us(2);
    PORT_SEC_REGS->GROUP[0].PORT_OUTSET = (1 << 17);  // TRIG high
    delay_us(10);
    PORT_SEC_REGS->GROUP[0].PORT_OUTCLR = (1 << 17);  // TRIG low again
    
    timeout = 30000;
    while ((PORT_SEC_REGS->GROUP[0].PORT_IN & (1 << 18)) == 0) { // wait for ECHO high
        timeout--;
        if (timeout == 0) return 99999; // timeout - no wall
    }
    
    timeout = 30000;
    while ((PORT_SEC_REGS->GROUP[0].PORT_IN & (1 << 18)) != 0) { // measure ECHO high time
        echo_time++;
        timeout--;
        if (timeout == 0) return 99999; // stuck high
    }
    
    return echo_time;
}

// ------------------------ READ FRONT SENSOR ------------------------
// PA00 (TRIG), PA01 (ECHO) - PORT A GROUP 0
uint32_t read_front_sensor_raw(void) {
    uint32_t echo_time = 0;
    uint32_t timeout;
    
    PORT_SEC_REGS->GROUP[0].PORT_OUTCLR = (1 << 0);  // TRIG low
    delay_us(2);
    PORT_SEC_REGS->GROUP[0].PORT_OUTSET = (1 << 0);  // TRIG high
    delay_us(10);
    PORT_SEC_REGS->GROUP[0].PORT_OUTCLR = (1 << 0);  // TRIG low
    
    timeout = 30000;
    while ((PORT_SEC_REGS->GROUP[0].PORT_IN & (1 << 1)) == 0) { // wait for ECHO
        timeout--;
        if (timeout == 0) return 99999;
    }
    
    timeout = 30000;
    while ((PORT_SEC_REGS->GROUP[0].PORT_IN & (1 << 1)) != 0) { // measure ECHO
        echo_time++;
        timeout--;
        if (timeout == 0) return 99999;
    }
    
    return echo_time;
}

// ------------------------ READ LEFT SENSOR ------------------------
// PB02 (TRIG), PB22 (ECHO) - PORT B GROUP 1
uint32_t read_left_sensor_raw(void) {
    uint32_t echo_time = 0;
    uint32_t timeout;
    
    PORT_SEC_REGS->GROUP[1].PORT_OUTCLR = (1 << 2);  // TRIG low (PORT B)
    delay_us(2);
    PORT_SEC_REGS->GROUP[1].PORT_OUTSET = (1 << 2);  // TRIG high
    delay_us(10);
    PORT_SEC_REGS->GROUP[1].PORT_OUTCLR = (1 << 2);  // TRIG low
    
    timeout = 30000;
    while ((PORT_SEC_REGS->GROUP[1].PORT_IN & (1 << 22)) == 0) { // wait for ECHO
        timeout--;
        if (timeout == 0) return 99999;
    }
    
    timeout = 30000;
    while ((PORT_SEC_REGS->GROUP[1].PORT_IN & (1 << 22)) != 0) { // measure ECHO
        echo_time++;
        timeout--;
        if (timeout == 0) return 99999;
    }
    
    return echo_time;
}

// ------------------------ INIT SENSOR PINS ------------------------
void init_sensor_pins(void) {
    // Right sensor
    PORT_SEC_REGS->GROUP[0].PORT_DIRSET = (1 << 17);  // PA17 = output (TRIG)
    PORT_SEC_REGS->GROUP[0].PORT_DIRCLR = (1 << 18);  // PA18 = input (ECHO)
    PORT_SEC_REGS->GROUP[0].PORT_PINCFG[18] = 0x02;   // enable digital input buffer
    
    // Front sensor
    PORT_SEC_REGS->GROUP[0].PORT_DIRSET = (1 << 0);   // PA00 = output (TRIG)
    PORT_SEC_REGS->GROUP[0].PORT_DIRCLR = (1 << 1);   // PA01 = input (ECHO)
    PORT_SEC_REGS->GROUP[0].PORT_PINCFG[1] = 0x02;    // enable digital input buffer
    
    // Left sensor (PORT B)
    PORT_SEC_REGS->GROUP[1].PORT_DIRSET = (1 << 2);   // PB02 = output (TRIG)
    PORT_SEC_REGS->GROUP[1].PORT_DIRCLR = (1 << 22);  // PB22 = input (ECHO)
    PORT_SEC_REGS->GROUP[1].PORT_PINCFG[22] = 0x02;   // enable digital input buffer
}

// ------------------------ RAW TO CM (RIGHT) ------------------------
uint32_t right_raw_to_cm(uint32_t raw) {
    if (raw <= 100) return 1;
    else if (raw <= 120) return 2;
    else if (raw <= 135) return 3;
    else if (raw <= 190) return 4;
    else if (raw <= 230) return 5;
    else if (raw <= 280) return 6;
    else if (raw <= 320) return 7;
    else if (raw <= 380) return 8;
    else if (raw <= 400) return 9;
    else if (raw <= 450) return 10;
    else return raw / 44;  // rough formula for >10cm
}

// ------------------------ RAW TO CM (FRONT) ------------------------
uint32_t front_raw_to_cm(uint32_t raw) {
    if (raw <= 100) return 1;
    else if (raw <= 120) return 2;
    else if (raw <= 150) return 3;
    else if (raw <= 230) return 4;
    else if (raw <= 280) return 5;
    else if (raw <= 320) return 6;
    else if (raw <= 380) return 7;
    else if (raw <= 410) return 8;
    else if (raw <= 470) return 9;
    else if (raw <= 520) return 10;
    else return raw / 44;
}

// ------------------------ RAW TO CM (LEFT) ------------------------
uint32_t left_raw_to_cm(uint32_t raw) {
    if (raw <= 85) return 1;
    else if (raw <= 110) return 2;
    else if (raw <= 130) return 3;
    else if (raw <= 190) return 4;
    else if (raw <= 220) return 5;
    else if (raw <= 260) return 6;
    else if (raw <= 300) return 7;
    else if (raw <= 350) return 8;
    else if (raw <= 430) return 9;
    else if (raw <= 470) return 10;
    else if (raw <= 520) return 11;
    else if (raw <= 560) return 12;
    else if (raw <= 600) return 13;
    else if (raw <= 650) return 14;
    else return raw / 44;
}

// ------------------------ SEND TO PUTTY ------------------------
void send_string(const char* str) {
    struct platform_ro_buf_desc desc = { str, strlen(str) };
    platform_usart_cdc_send_async(&desc, 1);
}

char sensor_buf[256];          // buffer for display text
uint32_t last_sensor_time = 0; // when we last updated display

// ------------------------ DECISION TO TEXT ------------------------
const char* get_decision_string(uint32_t action) {
    switch(action) {
        case 0: return "FORWARD";
        case 1: return "TURN RIGHT";
        case 2: return "TURN LEFT";
        case 3: return "TURN AROUND";
        default: return "UNKNOWN";
    }
}

// ------------------------ RIGHT-HAND RULE ------------------------
uint32_t get_right_hand_rule_action(uint32_t left_cm, uint32_t front_cm, uint32_t right_cm) {
    uint32_t left_open = (left_cm > SIDE_THRESHOLD_CM);   // left side open?
    uint32_t front_open = (front_cm > FRONT_THRESHOLD_CM); // front open?
    uint32_t right_open = (right_cm > SIDE_THRESHOLD_CM);  // right side open?
    
    if (right_open) return 1;      // no wall on right -> turn right to find it
    else if (front_open) return 0; // wall on right, path clear -> go forward
    else if (left_open) return 2;  // front+right blocked, left open -> turn left
    else return 3;                  // all blocked -> dead end, turn around
}

// ------------------------ PID MATH ------------------------
// returns: positive = turn right, negative = turn left, 0 = straight
int compute_pid_correction(uint32_t current_cm, float dt) {
    if (current_cm > 100) return 0; // no wall, don't steer
    
    float error = (float)current_cm - (float)DESIRED_WALL_DIST_CM; // how far off target?
    
    float proportional = KP_WALL * error;                           // P: react to current error
    integral += error * dt;                                         // I: remember past errors
    if (integral > 100) integral = 100;                            // anti-windup
    if (integral < -100) integral = -100;
    float integral_term = KI_WALL * integral;
    float derivative = KD_WALL * (error - previous_error) / dt;    // D: predict future
    previous_error = error;
    
    float output = proportional + integral_term + derivative;
    
    if (output > 12) output = 12;   // limit to ±12
    if (output < -12) output = -12;
    
    return (int)output;
}

// ------------------------ PID NUMBER TO TEXT ------------------------
const char* get_correction_string(int correction) {
    if (correction <= -8) return "(STEER HARD LEFT)";
    else if (correction < 0) return "(STEER LEFT)";
    else if (correction >= 8) return "(STEER HARD RIGHT)";
    else if (correction > 0) return "(STEER RIGHT)";
    else return "(CENTER)";
}



// -------- TEMPORARY MOTOR FUNCTIONS (FOR STANDALONE TESTING) ----------------
// -------- GITHUB INTEGRATION: DELETE when on GitHub ---
// -------- Then uncomment #include "locomotion.h" at the top ----------------


void go_forward(void) { }
void turn_right(void) { }
void turn_left(void) { }
void set_a_speed(int speed) { }
void set_b_speed(int speed) { }

// -------- END OF TEMPORARY MOTOR FUNCTIONS ---------------------------------




// ------------------------ MAIN ------------------------
int main(void)
{
    uint32_t right_raw, front_raw, left_raw;
    uint32_t current_time;
    
    platform_init_early();           // start clocks, etc.
    platform_PB_LED_init();          // setup button and LED
    platform_usart_cdc_init();       // start UART for PuTTY
    init_sensor_pins();              // setup sensor pins
    platform_init_late();            // finish init, enable interrupts
    
    send_string(banner_msg);         // show welcome message
    
    for (int i = 0; i < 3; i++) {    // blink LED 3x to show we're alive
        platform_LED_onboard_set();
        for (volatile int j = 0; j < 100000; j++);
        platform_LED_onboard_clear();
        for (volatile int j = 0; j < 100000; j++);
    }
    
    while (1) {
        current_time = platform_systick_count();
        
        if (current_time - last_sensor_time >= 20) { // update every 200ms
            last_sensor_time = current_time;
            
            float dt = 0.05;  // time since last PID calc (default 50ms)
            if (last_pid_time != 0) {
                dt = (float)(current_time - last_pid_time) * 0.01;
                if (dt > 0.1) dt = 0.05;
            }
            last_pid_time = current_time;
            
            // read all three sensors
            right_raw = read_right_sensor_raw();
            delay_ms(20);   
            front_raw = read_front_sensor_raw();
            delay_ms(20);
            left_raw  = read_left_sensor_raw();
            
            // convert to cm
            uint32_t right_cm = right_raw_to_cm(right_raw);
            uint32_t front_cm = front_raw_to_cm(front_raw);
            uint32_t left_cm  = left_raw_to_cm(left_raw);
            
            // decide what to do
            uint32_t action_code = get_right_hand_rule_action(left_cm, front_cm, right_cm);
            const char* decision_str = get_decision_string(action_code);
            
            // reset PID memory after a turn (prevents violent jerk)
            static uint32_t last_action = 99;
            if (action_code == 0 && last_action != 0) {
                integral = 0;
                previous_error = 0;
            }
            last_action = action_code;
            
            // calculate steering correction
            int pid_correction = 0;
            const char* correction_str = "(N/A)";
            
            if (action_code == 0) {
                if (right_cm <= 3) {                        // too close to wall
                    pid_correction = -12;                   // force hard left turn
                    correction_str = "TOO CLOSE! (HARD LEFT)";
                } 
                else if (right_cm < 100) {                  // normal operation
                    pid_correction = compute_pid_correction(right_cm, dt);
                    correction_str = get_correction_string(pid_correction);
                } 
                else {
                    correction_str = "(NO WALL)";
                }
            }
            

            // -------- MOTOR CONTROL LOGIC ------------------------------
            if (action_code == 0) {
                // FORWARD with smooth steering
                int left_speed = BASE_SPEED + pid_correction;
                int right_speed = BASE_SPEED - pid_correction;
                
                if (left_speed > MAX_SPEED) left_speed = MAX_SPEED;
                if (left_speed < MIN_SPEED) left_speed = MIN_SPEED;
                if (right_speed > MAX_SPEED) right_speed = MAX_SPEED;
                if (right_speed < MIN_SPEED) right_speed = MIN_SPEED;
                
                go_forward();
                set_a_speed(left_speed);
                set_b_speed(right_speed);
            }
            else if (action_code == 1) {
                // TURN RIGHT
                turn_right();
                set_a_speed(TURN_SPEED);
                set_b_speed(TURN_SPEED);
            }
            else if (action_code == 2) {
                // TURN LEFT
                turn_left();
                set_a_speed(TURN_SPEED);
                set_b_speed(TURN_SPEED);
            }
            else if (action_code == 3) {
                // TURN AROUND (180 degrees)
                turn_right();
                set_a_speed(TURN_SPEED);
                set_b_speed(TURN_SPEED);
                delay_ms(500);
                turn_right();
                set_a_speed(TURN_SPEED);
                set_b_speed(TURN_SPEED);
                delay_ms(500);
            }
            
            // -------- END OF MOTOR CONTROL LOGIC ------------------------

            
            // display everything on PuTTY (rows 6-10)
            snprintf(sensor_buf, sizeof(sensor_buf),
                     "\033[6;1HRight - Raw: %5lu  |  Dist: %2lu cm    "
                     "\033[7;1HFront - Raw: %5lu  |  Dist: %2lu cm    "
                     "\033[8;1HLeft  - Raw: %5lu  |  Dist: %2lu cm    "
                     "\033[9;1HDecision: %-12s                           "
                     "\033[10;1HPID Correction: %+4d  %-28s               ",
                     right_raw, right_cm,
                     front_raw, front_cm,
                     left_raw,  left_cm,
                     decision_str,
                     pid_correction, correction_str);
            
            send_string(sensor_buf);
        }
    }
    
    return 1;
}