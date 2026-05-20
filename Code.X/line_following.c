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

// SENSOR OUTPUT:
//       BLACK IS HIGH (1)
//       DETECTION/ WHITE IS LOW (0)

#define BASE_SPEED_A 28
#define BASE_SPEED_B 29

#define TURN_RIGHT_SPEED_A 25 // for sharper turn, increase A
#define TURN_RIGHT_SPEED_B 25

#define TURN_LEFT_SPEED_A 25 // for sharper turn, increase B
#define TURN_LEFT_SPEED_B 25

#define SLIGHT_TURN_RIGHT_SPEED_A 15 // for sharper turn, increase A
#define SLIGHT_TURN_RIGHT_SPEED_B 15

#define SLIGHT_TURN_LEFT_SPEED_A 15 // for sharper turn, increase B
#define SLIGHT_TURN_LEFT_SPEED_B 15

static int a_speed = 0;
static int b_speed = 0;


#define INTEGRAL_LIMIT 80
#define SEARCH_TIME 120
#define INTERSECTION_DELAY 120

static int last_error = 0;


bool ir_left(void);
bool ir_center(void);
bool ir_right(void);


void set_motors(int a_speed, int b_speed) {
    if ((TCC3_REGS->TCC_SYNCBUSY & 0x00000080) == 0) {
            TCC3_REGS->TCC_CCBUF[1] = a_speed;
            TCC3_REGS->TCC_CCBUF[0] = b_speed;
	}
}

void set_motors_delay(int a_speed, int b_speed, int delay) {
    for (int i = 0; i < delay; i++) {
        if ((TCC3_REGS->TCC_SYNCBUSY & 0x00000080) == 0) {
            TCC3_REGS->TCC_CCBUF[1] = a_speed;
            TCC3_REGS->TCC_CCBUF[0] = b_speed;
	}
    }
    return;
}

bool find_line() {
    if (ir_left() || ir_center() || ir_right()) {
        return true;
    }
    return false;
}

void line_following_algorithm(bool left, bool center, bool right)
{

    // 0 1 0  CENTER
    if(!left && center && !right)
    {
        go_forward();

        a_speed = BASE_SPEED_A;
        b_speed = BASE_SPEED_B;

        set_motors(a_speed, b_speed);

        last_error = 0;
        return;
    }
    
    // Intersection (1 1 1) ALL BLACK DETECTED
    // this is when BLACK is 1 1 1 and White is 0 0 0
    if(left && center && right)
    {   
        go_forward();
        a_speed = 28;
        b_speed = 31;
        set_motors(a_speed, b_speed);
        last_error = 0;
        return;  
    }
    
    if(left && !center && right)
    {
        // Force LEFT (1 0 1)
        turn_left();

        a_speed = SLIGHT_TURN_LEFT_SPEED_A;
        b_speed = SLIGHT_TURN_LEFT_SPEED_B;

        set_motors(a_speed, b_speed);
        last_error = -2;  //last direction
        return;
    }
        

    // Line Lost (0 0 0) ALL WHITE DETECTED
    if(!left && !center && !right)
    {   
        
        a_speed = 0;
        b_speed = 24;
        go_forward();
        set_motors(a_speed, b_speed);
        
        
        
        if (!find_line()) {
 
          
           for (int i = 0; i<2; i++) {
               for (int i = 0; i < 5000; i++) {
                   turn_right();
                   set_motors(a_speed, b_speed);
                   if (find_line()) {
                       return;
                   }
               }
               
           
               for (int i = 0; i < 5000; i++) {
                   turn_left();
                   set_motors(b_speed, a_speed);
                   if (find_line()) {
                       return;
                   }
               }
          
           }
           
           if (find_line()) {
                       return;
                   }
           
           
           a_speed = 24;
           b_speed = 24;
           for (int i = 0; i < 500000000; i++) {
           go_backward();
                set_motors(a_speed, b_speed);
                if (find_line()) {
                    return;
                }
           }
           // SAFE MODE HERE
        }
        
        return;
    }

    // 1 0 0  LEFT
    if(left && !center && !right)
    {
        turn_left();
            
        a_speed = TURN_LEFT_SPEED_A;
        b_speed = TURN_LEFT_SPEED_B;

        set_motors(a_speed, b_speed);
        last_error = -2;
        return;
    }

    // 1 1 0 line slightly LEFT
    if(left && center && !right)
    {
        turn_left();

        if (0) {
        a_speed = BASE_SPEED_A;
        b_speed = (BASE_SPEED_B + 5); //for slight turn
        }
        else {
            a_speed = SLIGHT_TURN_LEFT_SPEED_A;
            b_speed = SLIGHT_TURN_LEFT_SPEED_B;
        }

        set_motors(a_speed, b_speed);
        last_error = -1;
        return;
    }

    // 0 0 1  RIGHT
    if(!left && !center && right)
    {
        turn_right();

        a_speed = TURN_RIGHT_SPEED_A;
        b_speed = TURN_RIGHT_SPEED_B;

        set_motors(a_speed, b_speed);
        last_error = 2;
        return;
    }

    // 0 1 1 line slightly RIGHT
    if(!left && center && right)
    {
        turn_right();

        a_speed = SLIGHT_TURN_RIGHT_SPEED_A;
        b_speed = SLIGHT_TURN_RIGHT_SPEED_B;

        set_motors(a_speed, b_speed);
        last_error = 1;
        return;
    }
    
  
    
    go_forward();
    a_speed = BASE_SPEED_A;
    b_speed = BASE_SPEED_B;
    set_motors(a_speed, b_speed);
    return;

}
