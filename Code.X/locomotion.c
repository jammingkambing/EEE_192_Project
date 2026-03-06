/* 
 * File:   locomotion.c
 * Author: Jami
 *
 * Created on March 1, 2026, 1:29 AM
 */

#include "main.h"
#include "usart.h"
#include "init.h"
#include <xc.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

/*For reference: 
 * Motor driver pins:
PA06 - AIN1
PA03 - AIN2
PA02 - PWMA
PB03 - STBY
PB23 - BIN1
PA19 - BIN2
PA07 - PWMB
 */

//See TB6612FNG datasheet for truth table details

void stop(void) {
    // A = STOP
    PORT_SEC_REGS->GROUP[0].PORT_OUT &= ~(1 << 6); //IN1 = L
    PORT_SEC_REGS->GROUP[0].PORT_OUT &= ~(1 << 3); //IN2 = L
    PORT_SEC_REGS->GROUP[0].PORT_OUT |= (1 << 2); //PWM = H
    // B = STOP
    PORT_SEC_REGS->GROUP[1].PORT_OUT &= ~(1 << 23); //IN1 = L
    PORT_SEC_REGS->GROUP[0].PORT_OUT &= ~(1 << 19); //IN2 = L
    PORT_SEC_REGS->GROUP[0].PORT_OUT |= (1 << 7); //PWM = H
    
    PORT_SEC_REGS->GROUP[1].PORT_OUT |= (1 << 3); //STANDBY = H
}

void turn_right(void) {
    // A = STOP
    PORT_SEC_REGS->GROUP[0].PORT_OUT &= ~(1 << 6); //IN1 = L
    PORT_SEC_REGS->GROUP[0].PORT_OUT &= ~(1 << 3); //IN2 = L
    PORT_SEC_REGS->GROUP[0].PORT_OUT |= (1 << 2); //PWM = H
    // B = CW
    PORT_SEC_REGS->GROUP[1].PORT_OUT |= (1 << 23); //IN1 = H
    PORT_SEC_REGS->GROUP[0].PORT_OUT &= ~(1 << 19); //IN2 = L
    PORT_SEC_REGS->GROUP[0].PORT_OUT |= (1 << 7); //PWM = H
    PORT_SEC_REGS->GROUP[1].PORT_OUT |= (1 << 3); //STANDBY = H
}

void turn_left(void) {
    // A = CW
    PORT_SEC_REGS->GROUP[0].PORT_OUT |= (1 << 6); //IN1 = H
    PORT_SEC_REGS->GROUP[0].PORT_OUT &= ~(1 << 3); //IN2 = L
    PORT_SEC_REGS->GROUP[0].PORT_OUT |= (1 << 2); //PWM = H
    PORT_SEC_REGS->GROUP[1].PORT_OUT |= (1 << 3); //STANDBY = H
    // B = STOP
    PORT_SEC_REGS->GROUP[1].PORT_OUT &= ~(1 << 23); //IN1 = L
    PORT_SEC_REGS->GROUP[0].PORT_OUT &= ~(1 << 19); //IN2 = L
    PORT_SEC_REGS->GROUP[0].PORT_OUT |= (1 << 7); //PWM = H
}

void go_forward(void) {
    // A = CW
    PORT_SEC_REGS->GROUP[0].PORT_OUT |= (1 << 6); //IN1 = H
    PORT_SEC_REGS->GROUP[0].PORT_OUT &= ~(1 << 3); //IN2 = L
    PORT_SEC_REGS->GROUP[0].PORT_OUT |= (1 << 2); //PWM = H
    PORT_SEC_REGS->GROUP[1].PORT_OUT |= (1 << 3); //STANDBY = H
    // B = CW
    PORT_SEC_REGS->GROUP[1].PORT_OUT |= (1 << 23); //IN1 = H
    PORT_SEC_REGS->GROUP[0].PORT_OUT &= ~(1 << 19); //IN2 = L
    PORT_SEC_REGS->GROUP[0].PORT_OUT |= (1 << 7); //PWM = H
}

void go_backward(void) {
    // A = CCW
    PORT_SEC_REGS->GROUP[0].PORT_OUT &= ~(1 << 6); //IN1 = L
    PORT_SEC_REGS->GROUP[0].PORT_OUT |= (1 << 3); //IN2 = H
    PORT_SEC_REGS->GROUP[0].PORT_OUT |= (1 << 2); //PWM = H
    PORT_SEC_REGS->GROUP[1].PORT_OUT |= (1 << 3); //STANDBY = H
    // B = CCW
    PORT_SEC_REGS->GROUP[1].PORT_OUT &= ~(1 << 23); //IN1 = L
    PORT_SEC_REGS->GROUP[0].PORT_OUT |= (1 << 19); //IN2 = H
    PORT_SEC_REGS->GROUP[0].PORT_OUT |= (1 << 7); //PWM = H
}
