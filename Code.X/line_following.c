/* 
 * File:   line_following.c
 * Author: Jami
 *
 * Created on March 27, 2026, 2:15 PM
 */

#include "main.h"
#include "usart.h"
#include "init.h"
#include "locomotion.h"
#include <xc.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
 
#define BASE_SPEED 60
#define TURN_SPEED 40
#define MAX_SPEED 100
#define MIN_SPEED 0

#define KP 25
#define KI 5
#define KD 10

static int last_error = 0;
static int integral = 0;

void line_following_algorithm(bool left, bool center, bool right) {
    int error = 0;
    
    // not final pa since special case
    // Intersection (1 1 1)
    if(left && center && right){
        go_forward();
        set_a_speed(BASE_SPEED);
        set_b_speed(BASE_SPEED);
        return;
    }
    
    // Special Case (1 0 1)
    if(left && !center && right){
        // Choose left or right based on previous error
        if(last_error <= 0){ 
            turn_left();
            set_a_speed(TURN_SPEED);
            set_b_speed(BASE_SPEED);
        } else { 
            turn_right();
            set_a_speed(BASE_SPEED);
            set_b_speed(TURN_SPEED);
        }
        return;
    }
    
    // Normal Tracking
    if (left)
        error -= 1;
    
    if (right)
        error += 1;
    
    // Line Lost
    if(!left && !center && !right){
        if (last_error < 0) {
            turn_left();
            set_a_speed(TURN_SPEED);
            set_b_speed(TURN_SPEED);
        } else if(last_error > 0){
            turn_right();
            set_a_speed(TURN_SPEED);
            set_b_speed(TURN_SPEED);
        } else {
            go_forward();
            set_a_speed(TURN_SPEED);
            set_b_speed(TURN_SPEED);
        }
        return;
    }
    
    int integral += error;            
    int derivative = error - last_error;
    int correction = KP*error + KI*integral + KD*derivative;
    
    int speed_a = BASE_SPEED - correction;
    int speed_b = BASE_SPEED + correction;

    // Clamp speeds
    if(speed_a > MAX_SPEED) 
        speed_a = MAX_SPEED;
    if(speed_a < MIN_SPEED) 
        speed_a = MIN_SPEED;
    if(speed_b > MAX_SPEED) 
        speed_b = MAX_SPEED;
    if(speed_b < MIN_SPEED) 
        speed_b = MIN_SPEED;

    go_forward();
    set_a_speed(speed_a);
    set_b_speed(speed_b);

    last_error = error;
}

