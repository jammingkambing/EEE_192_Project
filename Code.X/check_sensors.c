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

// PIN DEFINITIONS
// FRONT SENSOR (PORT A)
#define FRONT_TRIG 0  // PA00
#define FRONT_ECHO 1  // PA01

// RIGHT SENSOR (PORT A)
#define RIGHT_TRIG 17 // PA17
#define RIGHT_ECHO 18 // PA18

// LEFT SENSOR (PORT B)
#define LEFT_TRIG 2   // PB02
#define LEFT_ECHO 22  // PB22

// HELPER FUNCTION DELAY
void delay_us(uint32_t us) {
    for (volatile uint32_t i = 0; i < (us * 5); i++); 
}

uint32_t read_sensor(uint8_t port_group, uint8_t trig_pin, uint8_t echo_pin) {
    uint32_t echo_time = 0;
    uint32_t timeout;

    // Clear trigger pin
    PORT_SEC_REGS->GROUP[port_group].PORT_OUTCLR = (1 << trig_pin);
    delay_us(2);
    
    // Send 10 microsecond pulse
    PORT_SEC_REGS->GROUP[port_group].PORT_OUTSET = (1 << trig_pin);
    delay_us(10);
    PORT_SEC_REGS->GROUP[port_group].PORT_OUTCLR = (1 << trig_pin);

    // Wait for echo pin to go HIGH
    timeout = 100000;
    while ((PORT_SEC_REGS->GROUP[port_group].PORT_IN & (1 << echo_pin)) == 0) {
        timeout--;
        if (timeout == 0) return 0;
    }

    // Measure how long echo pin stays HIGH
    timeout = 100000;
    while ((PORT_SEC_REGS->GROUP[port_group].PORT_IN & (1 << echo_pin)) != 0) {
        echo_time++;
        timeout--;
        if (timeout == 0) return 0;
    }

    return echo_time;
}

bool us_left(void){
    return false;
}

bool us_center(void) {
    return false;
}

bool us_right(void) {
    return false;
}

uint32_t front_dist(void) {
    return read_sensor(0, FRONT_TRIG, FRONT_ECHO);
}

uint32_t right_dist(void) {
    return read_sensor(0, RIGHT_TRIG, RIGHT_ECHO);
}

uint32_t left_dist(void) {
    read_sensor(0, FRONT_TRIG, FRONT_ECHO);
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
