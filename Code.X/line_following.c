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

// change this if hindi pa din stable 
#define KP 25
#define KI 5
#define KD 10

#define INTEGRAL_LIMIT 100
#define SEARCH_TIME 150   // ms
#define INTERSECTION_DELAY 150 // ms

static int last_error = 0;
static int integral = 0;

void line_following_algorithm(bool left, bool center, bool right) {
    int error = 0;
    
    // Intersection (1 1 1)
    if(left && center && right){
        
        integral = 0;
        stop();
        __delay_ms(INTERSECTION_DELAY);
        
        go_forward();
        set_a_speed(BASE_SPEED);
        set_b_speed(BASE_SPEED);
        return;
    }
    
    // Special Case (1 0 1)
    if(left && !center && right){
        
        integral = 0;
        
        // Choose left or right based on previous error
        if(last_error <= 0){ 
            turn_left();
            set_a_speed(TURN_SPEED);
            set_b_speed(BASE_SPEED);
            last_error = -2;
        } else { 
            turn_right();
            set_a_speed(BASE_SPEED);
            set_b_speed(TURN_SPEED);
            last_error = 2;
        }
        return;
    }
    
    // Line Lost
    if(!left && !center && !right){
        
        integral = 0;
        // search left first
        turn_left();
        set_a_speed(TURN_SPEED);
        set_b_speed(TURN_SPEED);
        __delay_ms(SEARCH_TIME);
        
        // read sensors again
        left = ir_left();
        center = ir_center();
        right = ir_right();
        
        if (left || center || right)
            return;
        
        // search right
        turn_right();
        set_a_speed(TURN_SPEED);
        set_b_speed(TURN_SPEED);
        __delay_ms(SEARCH_TIME);
        
        // read sensors again
        left = ir_left();
        center = ir_center();
        right = ir_right();

        if(left || center || right)
            return;
        
        // move forward
        go_forward();
        set_a_speed(TURN_SPEED);
        set_b_speed(TURN_SPEED);
        return;
    }
    
    // Normal Tracking
    if (left)
        error -= 2;
    
    if (right)
        error += 2;
    
    integral += error;      
    if(integral > INTEGRAL_LIMIT)
        integral = INTEGRAL_LIMIT;
    if(integral < -INTEGRAL_LIMIT)
        integral = -INTEGRAL_LIMIT;
    
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
