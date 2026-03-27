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
 * 
 */

bool us_left(void){
    if ((PORT_SEC_REGS->GROUP[1].PORT_IN & (1 << 22)) == 0) {
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
    
}

bool ir_center(void) {
    
}

bool ir_right(void) {
    
}
