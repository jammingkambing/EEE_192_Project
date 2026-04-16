/* 
 * File:   check_sensors.c
 * Author: Jami
 *
 * Created on March 27, 2026, 2:11 PM
 */

#include "main.h"
#include "usart.h"
#include "init.h"
#include <xc.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

/*For reference: 
 * Sensor pins:
 * R_TRIG = PA17
 * R_ECHO = PA18
 * F_TRIG = PA00
 * F_ECHO = PA01
 * L_TRIG = PA21
 * L_ECHO = PA22
 * ir_left = PA11
 * ir_center = PA10
 * ir_right = PA09
 */

bool us_left(void){
    if ((PORT_SEC_REGS->GROUP[0].PORT_IN & (1 << 11)) == 0) {
        return false;
    }
    else {
        return true;
    }
}

bool us_center(void) {
    
}

bool us_right(void) {
    
}

bool ir_left(void) {
    if ((PORT_SEC_REGS->GROUP[0].PORT_IN & (1 << 11)) == 0) {
        return false;
    }
    else {
        return true;
    }
}

bool ir_center(void) {
    if ((PORT_SEC_REGS->GROUP[0].PORT_IN & (1 << 10)) == 0) {
        return false;
    }
    else {
        return true;
    }
}

bool ir_right(void) {
    if ((PORT_SEC_REGS->GROUP[0].PORT_IN & (1 << 9)) == 0) {
        return false;
    }
    else {
        return true;
    }
}
