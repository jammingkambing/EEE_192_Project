
/*
 * ---------------------------------------------------------------------------
 * DARC BOT - WALL FOLLOWING SUBSYSTEM (SENSOR + DECISION + CONTROL)
 * Author: Kathleen Patajo
 * ---------------------------------------------------------------------------
 * 
 * MODIFIED: Right?sensor?only safe wall following + forward after every turn.
 *   - Uses only the right ultrasonic sensor for distance control.
 *   - Simple P?controller to keep ~5 cm from right wall.
 *   - If right wall disappears: turn right (one wheel stopped), then move forward.
 *   - Also for left turn and 180? turn: after turning, move forward to break loops.
 *   - Original Putty display format preserved.
 */

#include "init.h"
#include "usart.h"
#include "locomotion.h"
#include "main.h"
#include <xc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Motor control functions */
extern void go_forward(void);
extern void turn_left(void);
extern void turn_right(void);
extern void stop(void);

// ------------------------ THRESHOLDS ------------------------
#define SIDE_THRESHOLD_CM     20    // >15cm = no wall, turn right
#define FRONT_THRESHOLD_CM    12     // not used directly, kept for compatibility

// ------------------------ SIMPLE P?CONTROLLER SETTINGS ------------------------
#define DESIRED_WALL_DIST_CM  4     // target distance from right wall
#define P_STEER_STRENGTH      1     // max correction (?4%)
#define DEADBAND              1     // ignore errors within ?1 cm

// ------------------------ MOTOR SPEEDS (your calibrated values) ------------------------
#define BASE_SPEED_LEFT       25     // left wheel base speed (%)
#define BASE_SPEED_RIGHT      24    // right wheel base speed (%)
#define MAX_SPEED             40    // upper speed limit
#define MIN_SPEED             15    // lower speed limit

// ------------------------ TURN CALIBRATIONS (your values) ------------------------
#define TURN_RIGHT_MS         290   // 90? right turn duration (ms); 460; 700
#define TURN_RIGHT_SPEED      25    // PWM% during right turn (left wheel only)
#define TURN_LEFT_MS          30   // 90? left turn duration (ms)  last is 375
#define TURN_LEFT_SPEED       25    // PWM% during left turn (right wheel only)
#define TURN_AROUND_MS        220    // 180? turn duration (ms); 85
#define TURN_AROUND_SPEED     25    // PWM% during 180? turn

// Forward movement after any turn (anti?loop)
#define FORWARD_AFTER_TURN_MS 1000  // drive straight for 1 second after turn

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
//#define TURN_COOLDOWN_MS 300
//#define TURN_COOLDOWN_MS 300

// ------------------------ DELAYS ------------------------
static void delay_us(uint32_t us) {
    for (volatile uint32_t i = 0; i < (us * 5); i++);
}

void delay_ms(int ms)
{
    for(int i = 0; i < ms; i++)
    {
        for(volatile int j = 0; j < 4000; j++);
    }
}
// ------------------------ SENSOR READ FUNCTIONS (unchanged) ------------------------
uint32_t read_right_sensor_raw(void) {
    uint32_t echo_time = 0;
    uint32_t timeout;
    PORT_SEC_REGS->GROUP[1].PORT_OUTCLR = (1 << 2);
    delay_us(2);
    PORT_SEC_REGS->GROUP[1].PORT_OUTSET = (1 << 2);
    delay_us(10);
    PORT_SEC_REGS->GROUP[1].PORT_OUTCLR = (1 << 2);
    timeout = 5000;
    while ((PORT_SEC_REGS->GROUP[1].PORT_IN & (1 << 22)) == 0) {
        if (--timeout == 0) return 99999;
    }
    timeout = 5000;
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
    timeout = 5000;
    while ((PORT_SEC_REGS->GROUP[0].PORT_IN & (1 << 18)) == 0) {
        if (--timeout == 0) return 99999;
    }
    timeout = 5000;
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
    timeout = 5000;
    while ((PORT_SEC_REGS->GROUP[0].PORT_IN & (1 << 1)) == 0) {
        if (--timeout == 0) return 99999;
    }
    timeout = 5000;
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

// ------------------------ RAW TO CM (your calibrations) ------------------------
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

// ------------------------ SEND TO PUTTY (unchanged format) ------------------------
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

// ------------------------ DECISION TO TEXT (kept for display) ------------------------
const char* get_decision_string(uint32_t action) {
    switch(action) {
        case 0: return "FORWARD";
        case 1: return "TURN RIGHT";
        case 2: return "TURN LEFT";
        case 3: return "TURN AROUND";
        default: return "UNKNOWN";
    }
}

// ------------------------ RIGHT-HAND RULE (unused, but kept for compatibility) ------------------------
uint32_t get_right_hand_rule_action(uint32_t left_cm, uint32_t front_cm, uint32_t right_cm) {
    (void)left_cm; (void)front_cm; (void)right_cm;
    return 0; // not used
}

// ------------------------ SIMPLE P?CONTROLLER (replaces PID) ------------------------
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


// Safe mode parameters
#define SAFE_MODE_TIMEOUT_MS    10000   // 10 seconds of total stagnation before safe mode
#define SAFE_MODE_BLINK_MS      20      // fast blink (20ms on/off)

void safe_mode_led(int state) {
    static uint32_t last_blink = 0;
    static uint8_t led_state = 0;
    uint32_t current_time = platform_systick_count();
    
    if (state == 2) {
        // SAFE MODE: fast blink (20ms)
        if (platform_tick_delta(current_time, last_blink) >= SAFE_MODE_BLINK_MS) {
            last_blink = current_time;
            led_state = !led_state;
            if (led_state) {
                PORT_SEC_REGS->GROUP[0].PORT_OUTSET = (1 << 15);
            } else {
                PORT_SEC_REGS->GROUP[0].PORT_OUTCLR = (1 << 15);
            }
        }
    } else if (state == 1) {
        // Normal reverse: no blink (LED OFF)
        PORT_SEC_REGS->GROUP[0].PORT_OUTCLR = (1 << 15);
        led_state = 0;
    } else {
        // LED OFF
        PORT_SEC_REGS->GROUP[0].PORT_OUTCLR = (1 << 15);
        led_state = 0;
    }
}



// ------------------------ MAIN WALL FOLLOWING ALGORITHM (spin turns, no stop) ------------------------
// ------------------------ STAGNATION DETECTION (for physical jams) ------------------------
#define STAGNATION_TIMEOUT_MS   400   // if no sensor change for 500 ms -> reverse
#define STAGNATION_REVERSE_MS   400   // reverse duration (ms)
#define STAGNATION_REVERSE_SPEED_LEFT  25   // left wheel speed during reverse (%)
#define STAGNATION_REVERSE_SPEED_RIGHT 15   // right wheel speed during reverse (%)

// ------------------------ SAFE MODE LED (PA21) ------------------------
//#define LED_PIN 21
//#define LED_GROUP 0   // PORTA is GROUP[0]


void wall_following_algorithm(void) {
    static uint8_t initialized = 0;
    if (!initialized) {
        initialized = 1;
        init_sensor_pins();
        PORT_SEC_REGS->GROUP[0].PORT_DIRSET = (1 << 15);  // PA15 for safe mode LED
        safe_mode_led(0);
        send_string("\033[2J\033[H");   // clear screen once
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

    // Stagnation detection variables
    static uint32_t last_sensor_time = 0;
    static uint32_t last_right_cm = 0, last_front_cm = 0, last_left_cm = 0;
    static uint8_t stagnation_triggered = 0;
    
    // Safe mode tracking
    static uint32_t total_stagnation_time = 0;
    static uint8_t safe_mode_active = 0;

    uint32_t current_time = platform_systick_count();

    // Read sensors
    uint32_t right_raw = read_right_sensor_raw();
    uint32_t front_raw = read_front_sensor_raw();
    uint32_t left_raw  = read_left_sensor_raw();
    uint32_t right_cm = right_raw_to_cm(right_raw);
    uint32_t front_cm = front_raw_to_cm(front_raw);
    uint32_t left_cm  = left_raw_to_cm(left_raw);

    // ----- BLIND SPOT / TOO CLOSE DETECTION (right sensor) -----
    if (right_raw > 5000 || right_cm <= 2) {
        right_cm = 1;
    }

    // Stagnation detection
    #define STAG_TOLERANCE 1
    int right_diff = abs((int)right_cm - (int)last_right_cm);
    int front_diff = abs((int)front_cm - (int)last_front_cm);
    int left_diff  = abs((int)left_cm  - (int)last_left_cm);

    if (right_diff <= STAG_TOLERANCE && front_diff <= STAG_TOLERANCE && left_diff <= STAG_TOLERANCE) {
        if (last_sensor_time == 0) {
            last_sensor_time = current_time;
        } else if (platform_tick_delta(current_time, last_sensor_time) >= (STAGNATION_TIMEOUT_MS / PLATFORM_MS_PER_SYSTICK)) {
            if (!stagnation_triggered && !safe_mode_active) {
                // Add to total stagnation time
                total_stagnation_time += STAGNATION_TIMEOUT_MS;
                
                // Check if total stagnation exceeds safe mode threshold
                if (total_stagnation_time >= SAFE_MODE_TIMEOUT_MS) {
                    // ENTER SAFE MODE
                    safe_mode_active = 1;
                    stagnation_triggered = 1;
                    safe_mode_led(2);  // special code for safe mode (fast blink)
                    stop();
                    send_string(">>> SAFE MODE ACTIVATED! Robot stopped.\r\n");
                    send_string(">>> Reset to continue.\r\n");
                    // Stay in safe mode forever
                    while(1);
                } else {
                    // Normal reverse (no LED blink)
                    stagnation_triggered = 1;
                    go_backward();
                    set_b_speed(STAGNATION_REVERSE_SPEED_LEFT);
                    set_a_speed(STAGNATION_REVERSE_SPEED_RIGHT);
                    delay_ms(STAGNATION_REVERSE_MS);
                    stop();
                    last_sensor_time = 0;
                }
            }
        }
    } else {
        // Distances changed ? reset stagnation detection and timer
        last_right_cm = right_cm;
        last_front_cm = front_cm;
        last_left_cm = left_cm;
        last_sensor_time = 0;
        stagnation_triggered = 0;
        // Reset total stagnation time when moving again (only if not in safe mode)
        if (!safe_mode_active) {
            total_stagnation_time = 0;
        }
        safe_mode_led(0);
    }

    // ----- ANTI-SLAM (stronger left pivot) -----
    if (right_cm <= 4 && state != TURN_LEFT && state != TURN_RIGHT && state != TURN_AROUND) {
        turn_left(); 
        set_a_speed(10);     // stronger pivot
        set_b_speed(0); 
        delay_ms(80);        // longer pivot
        last_sensor_time = 0;
        stagnation_triggered = 0;
        safe_mode_led(0);
    }

    // Turn decisions
    if (front_cm <= FRONT_THRESHOLD_CM && right_cm <= SIDE_THRESHOLD_CM && left_cm <= SIDE_THRESHOLD_CM) {
        if (state != TURN_AROUND && state != TURN_LEFT && state != TURN_RIGHT) {
            state = TURN_AROUND;
            turn_ongoing = 1;
            turn_start_time = current_time;
            turn_left();
            set_b_speed(TURN_AROUND_SPEED);
            set_a_speed(TURN_AROUND_SPEED);
            last_sensor_time = 0;
            stagnation_triggered = 0;
            safe_mode_led(0);
        }
    }
    else if (front_cm <= FRONT_THRESHOLD_CM && right_cm <= SIDE_THRESHOLD_CM) {
        if (state != TURN_LEFT && state != TURN_RIGHT && state != TURN_AROUND) {
            state = TURN_LEFT;
            turn_ongoing = 1;
            turn_start_time = current_time;
            turn_left();
            set_b_speed(TURN_LEFT_SPEED);
            set_a_speed(TURN_LEFT_SPEED);
            last_sensor_time = 0;
            stagnation_triggered = 0;
            safe_mode_led(0);
        }
    }

    // Cooldown
    if (turn_cooldown_end != 0 && platform_tick_delta(current_time, turn_cooldown_end) >= 0)
        turn_cooldown_end = 0;

    switch (state) {
        case FOLLOW:
        {
            static float last_error = 0;
            int distance = right_cm;
            
            // Calculate error
            float error = (float)((int)DESIRED_WALL_DIST_CM - distance);
            
            // Higher P?gain (1.2) for faster reaction
            float p_term = error * 1.2f;
            
            // D?term for smoothness
            float d_term = (error - last_error) * 0.0f;
            last_error = error;
            
            float correction = p_term + d_term;
            
            // Very gentle limit: ?0.5%
            if (correction > 0.5f) correction = 0.5f;
            if (correction < -0.5f) correction = -0.5f;
            
            // Convert to integer for speed setting
            int left_speed = BASE_SPEED_LEFT + (int)(correction + 0.5f);
            int right_speed = BASE_SPEED_RIGHT - (int)(correction + 0.5f);
            
            // Clamp speeds
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
                    turn_right();
                    set_b_speed(TURN_RIGHT_SPEED);
                    set_a_speed(TURN_RIGHT_SPEED);
                    wall_lost_counter = 0;
                    last_sensor_time = 0;
                    stagnation_triggered = 0;
                    safe_mode_led(0);
                }
            } else {
                wall_lost_counter = 0;
            }
            break;
        }

        case TURN_RIGHT:
            if (turn_ongoing && platform_tick_delta(current_time, turn_start_time) >= (TURN_RIGHT_MS / PLATFORM_MS_PER_SYSTICK)) {
                turn_ongoing = 0; state = SEEK; turn_cooldown_end = current_time + (TURN_COOLDOWN_MS / PLATFORM_MS_PER_SYSTICK);
                last_sensor_time = 0;
                stagnation_triggered = 0;
                safe_mode_led(0);
            }
            break;

        case TURN_LEFT:
            if (turn_ongoing && platform_tick_delta(current_time, turn_start_time) >= (TURN_LEFT_MS / PLATFORM_MS_PER_SYSTICK)) {
                turn_ongoing = 0; state = SEEK; turn_cooldown_end = current_time + (TURN_COOLDOWN_MS / PLATFORM_MS_PER_SYSTICK);
                last_sensor_time = 0;
                stagnation_triggered = 0;
                safe_mode_led(0);
            }
            break;

        case TURN_AROUND:
            if (turn_ongoing && platform_tick_delta(current_time, turn_start_time) >= (TURN_AROUND_MS / PLATFORM_MS_PER_SYSTICK)) {
                turn_ongoing = 0; state = SEEK; turn_cooldown_end = current_time + (TURN_COOLDOWN_MS / PLATFORM_MS_PER_SYSTICK);
                last_sensor_time = 0;
                stagnation_triggered = 0;
                safe_mode_led(0);
            }
            break;

        case SEEK:
            go_forward(); set_b_speed(BASE_SPEED_LEFT); set_a_speed(BASE_SPEED_RIGHT);
            if (right_cm <= SIDE_THRESHOLD_CM) {
                seek_wall_detected_counter++;
                if (seek_wall_detected_counter >= SEEK_DEBOUNCE_COUNT) {
                    state = FOLLOW; seek_wall_detected_counter = 0;
                    last_sensor_time = 0;
                    stagnation_triggered = 0;
                    safe_mode_led(0);
                }
            } else {
                seek_wall_detected_counter = 0;
            }
            break;
    }

    // ----- PUTTY DISPLAY (sensors only on lines 20-22) -----
    const char* decision_str;
    switch (state) {
        case FOLLOW:     decision_str = "FOLLOW"; break;
        case TURN_RIGHT: decision_str = "TURN RIGHT"; break;
        case TURN_LEFT:  decision_str = "TURN LEFT"; break;
        case TURN_AROUND:decision_str = "TURN AROUND"; break;
        case SEEK:       decision_str = "SEEK"; break;
        default:         decision_str = "UNKNOWN"; break;
    }

    delay_ms(10);
}