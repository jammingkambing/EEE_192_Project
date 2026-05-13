
/*
 * ---------------------------------------------------------------------------
 * DARC BOT - WALL FOLLOWING SUBSYSTEM (SENSOR + Right-Hand Rule + CONTROL)
 * Author: Kathleen Patajo
 * ---------------------------------------------------------------------------
 */

#include "init.h"
#include "usart.h"
#include "locomotion.h"
#include "main.h"
#include <xc.h>
#include <string.h>
#include <stdio.h>

/* Motor control functions */
extern void go_forward(void);
extern void turn_left(void);
extern void turn_right(void);
extern void stop(void);

// ------------------------ THRESHOLDS ------------------------
#define SIDE_THRESHOLD_CM     15    // >15cm = no wall, turn right
#define FRONT_THRESHOLD_CM    12     

// ------------------------ P CONTROLLER SETTINGS ------------------------
#define DESIRED_WALL_DIST_CM  5     // target distance from right wall
#define P_STEER_STRENGTH      1     // max correction (±1%)
#define DEADBAND              2     // ignore errors within ±1 cm

// ------------------------ MOTOR SPEEDS  ------------------------
#define BASE_SPEED_LEFT       25     
#define BASE_SPEED_RIGHT      25    
#define MAX_SPEED             30    
#define MIN_SPEED             10    

// ------------------------ TURN CALIBRATIONS ------------------------
#define TURN_RIGHT_MS         250   // 90° right turn; 460; 700
#define TURN_RIGHT_SPEED      25    
#define TURN_LEFT_MS          115   // 90° left turn 
#define TURN_LEFT_SPEED       25    
#define TURN_AROUND_MS        450    // 180° turn; 85
#define TURN_AROUND_SPEED     25    


#define FORWARD_AFTER_TURN_MS 3000  

#define BALANCE_OFFSET        0

// ------------------------ LOCAL SPEED CONTROL ------------------------
#define MAX_PWM               99
#define MIN_EFFECTIVE_SPEED   10

int set_a_speed(int speed_percent) {
    int pwm;
    if (speed_percent < 0) speed_percent = 0;
    if (speed_percent > 100) speed_percent = 100;
    if (speed_percent > 0 && speed_percent < MIN_EFFECTIVE_SPEED)
        speed_percent = MIN_EFFECTIVE_SPEED;
    pwm = (speed_percent * MAX_PWM) / 100;
    if ((TCC3_REGS->TCC_SYNCBUSY & 0x00000080) == 0) {
        TCC3_REGS->TCC_CCBUF[1] = pwm;   // motor A = right wheel
    }
    return pwm;
}

int set_b_speed(int speed_percent) {
    int pwm;
    if (speed_percent < 0) speed_percent = 0;
    if (speed_percent > 100) speed_percent = 100;
    if (speed_percent > 0 && speed_percent < MIN_EFFECTIVE_SPEED)
        speed_percent = MIN_EFFECTIVE_SPEED;
    pwm = (speed_percent * MAX_PWM) / 100;
    if ((TCC3_REGS->TCC_SYNCBUSY & 0x00000080) == 0) {
        TCC3_REGS->TCC_CCBUF[0] = pwm;   // motor B = left wheel
    }
    return pwm;
}

// ------------------------ DEBOUNCE & COOLDOWN ------------------------
static uint8_t right_open_counter = 0;
#define RIGHT_OPEN_DEBOUNCE 1
static uint32_t turn_cooldown_end = 0;


// ------------------------ DELAYS ------------------------
static void delay_us(uint32_t us) {
    for (volatile uint32_t i = 0; i < (us * 5); i++);
}

// ------------------------ SENSOR READ FUNCTIONS ------------------------
uint32_t read_right_sensor_raw(void) {
    uint32_t echo_time = 0;
    uint32_t timeout;
    PORT_SEC_REGS->GROUP[1].PORT_OUTCLR = (1 << 2);
    delay_us(2);
    PORT_SEC_REGS->GROUP[1].PORT_OUTSET = (1 << 2);
    delay_us(10);
    PORT_SEC_REGS->GROUP[1].PORT_OUTCLR = (1 << 2);
    timeout = 30000;
    while ((PORT_SEC_REGS->GROUP[1].PORT_IN & (1 << 22)) == 0) {
        if (--timeout == 0) return 99999;
    }
    timeout = 30000;
    while ((PORT_SEC_REGS->GROUP[1].PORT_IN & (1 << 22)) != 0) {
        echo_time++;
        if (--timeout == 0) return 99999;
    }
    return echo_time;
}

uint32_t read_front_sensor_raw(void) {
    uint32_t echo_time = 0;
    uint32_t timeout;
    PORT_SEC_REGS->GROUP[0].PORT_OUTCLR = (1 << 17);
    delay_us(2);
    PORT_SEC_REGS->GROUP[0].PORT_OUTSET = (1 << 17);
    delay_us(10);
    PORT_SEC_REGS->GROUP[0].PORT_OUTCLR = (1 << 17);
    timeout = 30000;
    while ((PORT_SEC_REGS->GROUP[0].PORT_IN & (1 << 18)) == 0) {
        if (--timeout == 0) return 99999;
    }
    timeout = 30000;
    while ((PORT_SEC_REGS->GROUP[0].PORT_IN & (1 << 18)) != 0) {
        echo_time++;
        if (--timeout == 0) return 99999;
    }
    return echo_time;
}

uint32_t read_left_sensor_raw(void) {
    uint32_t echo_time = 0;
    uint32_t timeout;
    PORT_SEC_REGS->GROUP[0].PORT_OUTCLR = (1 << 0);
    delay_us(2);
    PORT_SEC_REGS->GROUP[0].PORT_OUTSET = (1 << 0);
    delay_us(10);
    PORT_SEC_REGS->GROUP[0].PORT_OUTCLR = (1 << 0);
    timeout = 30000;
    while ((PORT_SEC_REGS->GROUP[0].PORT_IN & (1 << 1)) == 0) {
        if (--timeout == 0) return 99999;
    }
    timeout = 30000;
    while ((PORT_SEC_REGS->GROUP[0].PORT_IN & (1 << 1)) != 0) {
        echo_time++;
        if (--timeout == 0) return 99999;
    }
    return echo_time;
}

void init_sensor_pins(void) {
    // FRONT
    PORT_SEC_REGS->GROUP[0].PORT_DIRSET = (1 << 17);
    PORT_SEC_REGS->GROUP[0].PORT_DIRCLR = (1 << 18);
    PORT_SEC_REGS->GROUP[0].PORT_PINCFG[18] = 0x02;
    // LEFT
    PORT_SEC_REGS->GROUP[0].PORT_DIRSET = (1 << 0);
    PORT_SEC_REGS->GROUP[0].PORT_DIRCLR = (1 << 1);
    PORT_SEC_REGS->GROUP[0].PORT_PINCFG[1] = 0x02;
    // RIGHT
    PORT_SEC_REGS->GROUP[1].PORT_DIRSET = (1 << 2);
    PORT_SEC_REGS->GROUP[1].PORT_DIRCLR = (1 << 22);
    PORT_SEC_REGS->GROUP[1].PORT_PINCFG[22] = 0x02;
}

// ------------------------ RAW TO CM (calibrated) ------------------------
uint32_t right_raw_to_cm(uint32_t raw) {
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

uint32_t front_raw_to_cm(uint32_t raw) {
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
    else return raw / 44;
}

uint32_t left_raw_to_cm(uint32_t raw) {
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

// ------------------------ SEND TO PUTTY ------------------------
void send_string(const char* str) {
    while (platform_usart_cdc_tx_busy()) {
        for (volatile int i = 0; i < 100; i++);
    }
    struct platform_ro_buf_desc desc = { str, strlen(str) };
    platform_usart_cdc_send_async(&desc, 1);
    while (platform_usart_cdc_tx_busy()) {
        for (volatile int i = 0; i < 100; i++);
    }
}

// ------------------------ DECISION TO TEXT (display) ------------------------
const char* get_decision_string(uint32_t action) {
    switch(action) {
        case 0: return "FORWARD";
        case 1: return "TURN RIGHT";
        case 2: return "TURN LEFT";
        case 3: return "TURN AROUND";
        default: return "UNKNOWN";
    }
}

// ------------------------ RIGHT-HAND RULE (not used, ayaw ko lang tanggalin) ------------------------
uint32_t get_right_hand_rule_action(uint32_t left_cm, uint32_t front_cm, uint32_t right_cm) {
    (void)left_cm; (void)front_cm; (void)right_cm;
    return 0; // not used
}

// ------------------------ SIMPLE P CONTROLLER (mahirap pag PID; di naman need buo) ------------------------
int compute_pid_correction(uint32_t current_cm, float dt, float *integral, float *previous_error) {
    (void)dt; (void)integral; (void)previous_error;
    if (current_cm > 100) return 0;
    int error = (int)current_cm - (int)DESIRED_WALL_DIST_CM;
    int correction = 0;
    if (error > DEADBAND)
        correction = P_STEER_STRENGTH;
    else if (error < -DEADBAND)
        correction = -P_STEER_STRENGTH;
    else
        correction = 0;
    return correction;
}

const char* get_correction_string(int correction) {
    if (correction <= -8) return "(STEER HARD LEFT)";
    else if (correction < 0) return "(STEER LEFT)";
    else if (correction >= 8) return "(STEER HARD RIGHT)";
    else if (correction > 0) return "(STEER RIGHT)";
    else return "(CENTER)";
}

// ------------------------ ANTI SLAM / BLIND SPOT ------------------------
#define BLIND_RAW_THRESH      5000
#define ANTI_SLAM_DURATION   20    // for pivot or reverse
#define ANTI_SLAM_SPEED      10    
#define ANTI_SLAM_REVERSE_SPEED 8  


// ------------------------ STUCK DETECTION ------------------------
#define STUCK_THRESHOLD       50      
#define STUCK_RECOVER_DURATION 100    // for reverse+turn
#define STUCK_RECOVER_SPEED   10

// ------------------------ SEEK TIMEOUT ------------------------
#define SEEK_TIMEOUT_MS       3000    // stop after 3 sec without wall

#define BALANCE_OFFSET        0


// ------------------------ MAIN WALL FOLLOWING ALGORITHM (spin turns, anti-slam, stuck detect) ------------------------
void wall_following_algorithm(void) {
    static uint8_t initialized = 0;
    if (!initialized) {
        initialized = 1;
        init_sensor_pins();
        send_string("\033[2J\033[H");               // clear screen once
        send_string("\r\n=== RIGHT HAND RULE (SPIN TURNS, ANTI-SLAM, STUCK DETECT) ===\r\n");
        //delay_ms(100);
    }

    static enum { FOLLOW, TURN_RIGHT, TURN_LEFT, TURN_AROUND, SEEK } state = FOLLOW;
    static uint32_t turn_start_time = 0;
    static uint8_t turn_ongoing = 0;
    static uint8_t seek_wall_detected_counter = 0;
    #define SEEK_DEBOUNCE_COUNT 3
    static uint8_t wall_lost_counter = 0;
    #define WALL_LOST_DEBOUNCE 2
    static uint32_t turn_cooldown_end = 0;
    #define TURN_COOLDOWN_MS 500
    static uint32_t seek_start_time = 0;

    // Stuck detection
    static uint32_t stuck_counter = 0;
    static uint32_t last_right_cm = 999;

    uint32_t current_time = platform_systick_count();

    // Read raw values
    uint32_t right_raw = read_right_sensor_raw();
    uint32_t front_raw = read_front_sensor_raw();
    uint32_t left_raw  = read_left_sensor_raw();
    uint32_t right_cm = right_raw_to_cm(right_raw);
    uint32_t front_cm = front_raw_to_cm(front_raw);
    uint32_t left_cm  = left_raw_to_cm(left_raw);

    // Blind spot / close range - treat as 1cm
    if (right_raw > BLIND_RAW_THRESH || right_cm <= 3) right_cm = 1;
    if (front_raw > BLIND_RAW_THRESH || front_cm <= 3) front_cm = 1;
    if (left_raw  > BLIND_RAW_THRESH || left_cm  <= 3) left_cm  = 1;

    // Anti-slam

    if (front_cm <= 2 && state != TURN_LEFT && state != TURN_RIGHT && state != TURN_AROUND) {
        turn_right();
        set_b_speed(-ANTI_SLAM_REVERSE_SPEED);
        set_a_speed(-ANTI_SLAM_REVERSE_SPEED);
        //delay_ms(ANTI_SLAM_DURATION);
        // stop();
        send_string("ANTI-SLAM: front reverse\r\n");
        //delay_ms(20);
        return;
    }
    if (left_cm <= 2 && state != TURN_LEFT && state != TURN_RIGHT && state != TURN_AROUND) {
        turn_right();
        set_b_speed(ANTI_SLAM_SPEED);
        set_a_speed(0);
        //delay_ms(ANTI_SLAM_DURATION);
        send_string("ANTI-SLAM: left pivot right\r\n");
        //delay_ms(20);
        return;
    }
    if (right_cm <= 2 && state != TURN_LEFT && state != TURN_RIGHT && state != TURN_AROUND) {
        turn_left();
        set_b_speed(0);
        set_a_speed(ANTI_SLAM_SPEED);
        //delay_ms(ANTI_SLAM_DURATION);
        send_string("ANTI-SLAM: right pivot left\r\n");
        //delay_ms(20);
        return;
    }



    if (front_cm <= FRONT_THRESHOLD_CM && right_cm <= SIDE_THRESHOLD_CM && left_cm <= SIDE_THRESHOLD_CM) {
        if (state != TURN_AROUND && state != TURN_LEFT && state != TURN_RIGHT) {
            send_string(">>> DEAD END -> 180° SPIN <<<\r\n");
            state = TURN_AROUND;
            turn_ongoing = 1;
            turn_start_time = current_time;
            turn_right();
            set_b_speed(TURN_AROUND_SPEED);
            set_a_speed(TURN_AROUND_SPEED);
        }
    }
    else if (front_cm <= FRONT_THRESHOLD_CM && right_cm <= SIDE_THRESHOLD_CM) {
        if (state != TURN_LEFT && state != TURN_RIGHT && state != TURN_AROUND) {
            send_string(">>> FRONT+RIGHT -> LEFT SPIN <<<\r\n");
            state = TURN_LEFT;
            turn_ongoing = 1;
            turn_start_time = current_time;
            turn_left();
            set_b_speed(TURN_LEFT_SPEED);
            set_a_speed(TURN_LEFT_SPEED);
        }
    }

    // Cooldown 

    if (turn_cooldown_end != 0 && platform_tick_delta(current_time, turn_cooldown_end) >= 0)
        turn_cooldown_end = 0;



    switch (state) {
        case FOLLOW:
        {
            // Stuck detection (right distance not changing)
            if (right_cm == last_right_cm) {
                stuck_counter++;
                if (stuck_counter >= STUCK_THRESHOLD && turn_cooldown_end == 0 && turn_ongoing == 0 && state == FOLLOW) {
                    send_string(">>> STUCK DETECTED! Recovering...\r\n");
                    turn_right();
                    set_b_speed(-STUCK_RECOVER_SPEED);
                    set_a_speed(-STUCK_RECOVER_SPEED);
                    //delay_ms(STUCK_RECOVER_DURATION);
                    turn_left();
                    set_b_speed(0);
                    set_a_speed(10);
                    //delay_ms(STUCK_RECOVER_DURATION);
                    //stop();
                    stuck_counter = 0;
                    turn_cooldown_end = current_time + (TURN_COOLDOWN_MS / PLATFORM_MS_PER_SYSTICK);
                    break;
                }
            } else {
                stuck_counter = 0;
                last_right_cm = right_cm;
            }

            // P?controller with deadband
            int error = (int)DESIRED_WALL_DIST_CM - (int)right_cm;
            if (error > -2 && error < 2) error = 0;
            int correction = (int)(error * 0.05f);
            if (correction > 1) correction = 1;
            if (correction < -1) correction = -1;

            int left_speed = BASE_SPEED_LEFT + correction;
            int right_speed = BASE_SPEED_RIGHT - correction;

            if (left_speed > MAX_SPEED) left_speed = MAX_SPEED;
            if (left_speed < MIN_SPEED) left_speed = MIN_SPEED;
            if (right_speed > MAX_SPEED) right_speed = MAX_SPEED;
            if (right_speed < MIN_SPEED) right_speed = MIN_SPEED;

            go_forward();
            set_b_speed(left_speed);
            set_a_speed(right_speed);

            // Right wall loss (debounced)
            if (right_cm > SIDE_THRESHOLD_CM) {
                wall_lost_counter++;
                if (wall_lost_counter >= WALL_LOST_DEBOUNCE && turn_cooldown_end == 0 && state == FOLLOW) {
                    state = TURN_RIGHT;
                    turn_ongoing = 1;
                    turn_start_time = current_time;
                    send_string("\nWall lost -> right spin\r\n");
                    turn_right();
                    set_b_speed(TURN_RIGHT_SPEED);
                    set_a_speed(TURN_RIGHT_SPEED);
                    wall_lost_counter = 0;
                }
            } else {
                wall_lost_counter = 0;
            }
            break;
        }

        case TURN_RIGHT:
            if (turn_ongoing && platform_tick_delta(current_time, turn_start_time) >= (TURN_RIGHT_MS / PLATFORM_MS_PER_SYSTICK)) {
                turn_ongoing = 0; state = SEEK; turn_cooldown_end = current_time + (TURN_COOLDOWN_MS / PLATFORM_MS_PER_SYSTICK);
                send_string("Right spin done -> SEEK\r\n");
                seek_start_time = current_time;
                stuck_counter = 0;
            }
            break;

        case TURN_LEFT:
            if (turn_ongoing && platform_tick_delta(current_time, turn_start_time) >= (TURN_LEFT_MS / PLATFORM_MS_PER_SYSTICK)) {
                turn_ongoing = 0; state = SEEK; turn_cooldown_end = current_time + (TURN_COOLDOWN_MS / PLATFORM_MS_PER_SYSTICK);
                send_string("Left spin done -> SEEK\r\n");
                seek_start_time = current_time;
                stuck_counter = 0;
            }
            break;

        case TURN_AROUND:
            if (turn_ongoing && platform_tick_delta(current_time, turn_start_time) >= (TURN_AROUND_MS / PLATFORM_MS_PER_SYSTICK)) {
                turn_ongoing = 0; state = SEEK; turn_cooldown_end = current_time + (TURN_COOLDOWN_MS / PLATFORM_MS_PER_SYSTICK);
                send_string("180° spin done -> SEEK\r\n");
                seek_start_time = current_time;
                stuck_counter = 0;
            }
            break;

        case SEEK:
            go_forward(); set_b_speed(BASE_SPEED_LEFT); set_a_speed(BASE_SPEED_RIGHT);
            send_string("SEEK mode\r");
            if (right_cm <= SIDE_THRESHOLD_CM) {
                seek_wall_detected_counter++;
                if (seek_wall_detected_counter >= SEEK_DEBOUNCE_COUNT) {
                    state = FOLLOW; seek_wall_detected_counter = 0;
                    send_string("Wall found -> FOLLOW\r\n");
                }
            } else {
                seek_wall_detected_counter = 0;
            }
            if (platform_tick_delta(current_time, seek_start_time) >= (SEEK_TIMEOUT_MS / PLATFORM_MS_PER_SYSTICK)) {
                // stop();
                send_string("SEEK timeout: no wall found, stopping\r\n");
               // while(1);
            }
            break;
    }

    // PUTTY DISPLAY 

    const char* decision_str;
    switch (state) {
        case FOLLOW:   decision_str = "FOLLOW"; break;
        case TURN_RIGHT: decision_str = "TURN RIGHT"; break;
        case TURN_LEFT:  decision_str = "TURN LEFT"; break;
        case TURN_AROUND:decision_str = "TURN AROUND"; break;
        case SEEK:       decision_str = "SEEK"; break;
        default:         decision_str = "UNKNOWN"; break;
    }

    // same P?controller correction variable 
    int disp_correction = 0;
    const char* disp_corr_str = "(N/A)";
    if (state == FOLLOW) {
        int error = (int)DESIRED_WALL_DIST_CM - (int)right_cm;
        if (error > -2 && error < 2) error = 0;
        disp_correction = (int)(error * 0.05f);
        if (disp_correction > 1) disp_correction = 1;
        if (disp_correction < -1) disp_correction = -1;
        disp_corr_str = get_correction_string(disp_correction);
    } else {
        disp_corr_str = "(N/A)";
    }

    char sensor_buf[256];
    snprintf(sensor_buf, sizeof(sensor_buf),
             "\033[20;1HRight - Raw: %5lu  |  Dist: %2lu cm    "
             "\033[21;1HFront - Raw: %5lu  |  Dist: %2lu cm    "
             "\033[22;1HLeft  - Raw: %5lu  |  Dist: %2lu cm    "
             "\033[23;1HDecision: %-12s                           "
             "\033[24;1HPID Correction: %+4d  %-28s               ",
             right_raw, right_cm,
             front_raw, front_cm,
             left_raw,  left_cm,
             decision_str,
             disp_correction, disp_corr_str);

    send_string(sensor_buf);
    

    delay_ms(5);
}