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

#define BASE_SPEED 65
#define TURN_SPEED 45
#define MAX_SPEED 100
#define MIN_SPEED 30

// not verified
#define KP 22
#define KI 3
#define KD 12

#define INTEGRAL_LIMIT 80
#define SEARCH_TIME 120
#define INTERSECTION_DELAY 120

static int last_error = 0;
static int integral = 0;

bool ir_left(void);
bool ir_center(void);
bool ir_right(void);

void line_following_algorithm(bool left, bool center, bool right)
{
    int error = 0;

    // Intersection (1 1 1) 
    // this is when BLACK is 1 1 1 and White is 0 0 0
    if(left && center && right)
    {
        delay_ms(20); // filter noise

        if(ir_left() && ir_center() && ir_right())
        {
            integral = 0;
            last_error = 0;

            stop();
            delay_ms(INTERSECTION_DELAY);

            go_forward();
            set_a_speed(BASE_SPEED);
            set_b_speed(BASE_SPEED);
            return;
        }
    }
    
    // Line Lost
    if(!left && !center && !right)
    {
        integral = 0;

        for(int i = 0; i < 3; i++)
        {
            turn_left();
            set_a_speed(TURN_SPEED);
            set_b_speed(TURN_SPEED);
            delay_ms(SEARCH_TIME);

            if(ir_left() || ir_center() || ir_right())
                return;

            turn_right();
            set_a_speed(TURN_SPEED);
            set_b_speed(TURN_SPEED);
            delay_ms(SEARCH_TIME);

            if(ir_left() || ir_center() || ir_right())
                return;
        }

        go_forward();
        set_a_speed(TURN_SPEED);
        set_b_speed(TURN_SPEED);
        return;
    }

    // Error
    if(left && !center && !right) error = -2;
    else if(left && center && !right) error = -1;
    else if(!left && center && !right) error = 0;
    else if(!left && center && right) error = 1;
    else if(!left && !center && right) error = 2;

    // PID
    integral += error;

    if(integral > INTEGRAL_LIMIT) integral = INTEGRAL_LIMIT;
    if(integral < -INTEGRAL_LIMIT) integral = -INTEGRAL_LIMIT;

    if(error == 0)
        integral = 0;

    int derivative = error - last_error;

    int correction = KP*error + KI*integral + KD*derivative;

    int speed_a = BASE_SPEED - correction;
    int speed_b = BASE_SPEED + correction;


    if(speed_a > MAX_SPEED) speed_a = MAX_SPEED;
    if(speed_a < MIN_SPEED) speed_a = MIN_SPEED;

    if(speed_b > MAX_SPEED) speed_b = MAX_SPEED;
    if(speed_b < MIN_SPEED) speed_b = MIN_SPEED;

    go_forward();
    set_a_speed(speed_a);
    set_b_speed(speed_b);

    last_error = error;
}


// TO DO/REMINDERS

// WALA PA YUNG VOID SET_A_SPEED(INT SPEED) sa locomotion.c or main.c

// SENSOR OUTPUT:
//       NO DETECTION/BLACK IS HIGH (1)
//       DETECTION/ WHITE IS LOW (0)

// in the sensor test,
//    WHITE is LOW (0)
//    BLACK is HIGH (1)
//    NOTHING or FAR is HIGH (1)







// OLD VERSION 
//    // Special Case (1 0 1)
//    if(left && !center && right){
//        
//        integral = 0;
//        
//        // Choose left or right based on previous error
//        if(last_error <= 0){ 
//            turn_left();
//            set_a_speed(TURN_SPEED);
//            set_b_speed(BASE_SPEED);
//            last_error = -2;
//        } else { 
//            turn_right();
//            set_a_speed(BASE_SPEED);
//            set_b_speed(TURN_SPEED);
//            last_error = 2;
//        }
//        return;
//    }


