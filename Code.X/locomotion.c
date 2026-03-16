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
PA06 - PWMA
PA03 - AIN2
PA02 - AIN1
PB03 - STBY
PB23 - BIN1
PA19 - BIN2
PA07 - PWMB
 */

//See TB6612FNG datasheet for truth table details

// Speed variables are to be changed according to calibration.

void b_stop(void) {
    PORT_SEC_REGS->GROUP[0].PORT_OUT &= ~(1 << 3); //IN2 = L
    PORT_SEC_REGS->GROUP[0].PORT_OUT &= ~(1 << 2); //IN1 = L
}

void a_stop(void) {
    PORT_SEC_REGS->GROUP[1].PORT_OUT &= ~(1 << 23); //IN1 = L
    PORT_SEC_REGS->GROUP[0].PORT_OUT &= ~(1 << 19); //IN2 = L
}

void b_cw(void) {
    PORT_SEC_REGS->GROUP[0].PORT_OUT &= ~(1 << 3); //IN2 = L
    PORT_SEC_REGS->GROUP[0].PORT_OUT |= (1 << 2); // IN1 = H
}

void a_cw(void) {
    PORT_SEC_REGS->GROUP[1].PORT_OUT |= (1 << 23); //IN1 = H
    PORT_SEC_REGS->GROUP[0].PORT_OUT &= ~(1 << 19); //IN2 = L
}

void b_ccw(void) {
    PORT_SEC_REGS->GROUP[0].PORT_OUT |= (1 << 3); //IN2 = H
    PORT_SEC_REGS->GROUP[0].PORT_OUT &= ~(1 << 2); //IN1 = L
}

void a_ccw(void) {
    PORT_SEC_REGS->GROUP[1].PORT_OUT &= ~(1 << 23); //IN1 = L
    PORT_SEC_REGS->GROUP[0].PORT_OUT |= (1 << 19); //IN2 = H
}

void stop(void) {
    PORT_SEC_REGS->GROUP[1].PORT_OUT |= (1 << 3); //STANDBY = H
    a_stop();
    b_stop();
}

void turn_right(void) {
    PORT_SEC_REGS->GROUP[1].PORT_OUT |= (1 << 3); //STANDBY = H
    a_cw();
    b_ccw();
}

void turn_left(void) {
    PORT_SEC_REGS->GROUP[1].PORT_OUT |= (1 << 3); //STANDBY = H
    a_ccw();
    b_cw();
}

void go_forward(void) {
    PORT_SEC_REGS->GROUP[1].PORT_OUT |= (1 << 3); //STANDBY = H
    a_ccw();
    b_ccw();
}

void go_backward(void) {
    PORT_SEC_REGS->GROUP[1].PORT_OUT |= (1 << 3); //STANDBY = H
    a_cw();
    b_cw();
}
