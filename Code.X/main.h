/**
 * @file main.h
 *
 * Initial version:  EEE 158 AY2025-26 Sem 1 Handlers
 * Modifications by: Jam Eclarin
 *
 * Prototypes for functions defined in platform.c and irq_handlers.c,
 * and called from main.c
 */

/*
 * The outermost #if-#endif block makes sure that if this header file is
 * included multiple times (virtually always the case in production code), we
 * don't get multiple-definition errors from the compiler.
 *
 * (C/C++ Standard, "One-definition Rule" [ODR])
 */
#ifndef EEE158_MAIN_H
#define EEE158_MAIN_H

#include "eee158_hplib.h"

#include <stdint.h>
#include <limits.h>

/*
 * The "extern 'C'" block makes sure that, if this file gets included from
 * C++, name mangling is not performed.
 *
 * (You can search the C++ standard to find out about name mangling.)
 */
#ifdef __cplusplus
extern "C" {
#endif

	/*
	 * TODO: Add bitmasks for platform_evt_add() and platform_evt_get()
	 * 
	 * NOTE: Bitmask values must have exactly one bit set to '1'; all
	 *       others must be cleared to '0'.
	 */
	
/// If set, the on-board button has been pressed
#if !defined(__cplusplus) && __STDC_VERSION__ < 202311l
#define bool _Bool
#define true (1)
#define false (0)
#endif

/*
 * The "extern 'C'" block makes sure that, if this file gets included from
 * C++, name mangling is not performed.
 *
 * (You can search the C++ standard to find out about name mangling.)
 */
#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * Perform early platform initialization
	 * 
	 * @details
	 * The initialization phase is split into "early" and "late" parts.
	 * Once the full initialization process is complete, interrupts are
	 * enabled and the code must be ready to process them anytime. For this
	 * reason, the application must initialize all it needs between this
	 * function and platform_init_late().
	 * 
	 * @remarks
	 * Upon return from this function, the platform will have the following
	 * characteristics:
	 *	- CPU clock boosted to 24 MHz;
	 *	- GCLK_GEN2 configured and enabled for 4 MHz from OSC16M;
	 *	- EIC & EVSYS reset; and
	 *	- EIC's generic-clock input enabled.
	 */
	extern void platform_init_early(void);
	
	/**
	 * Perform late platform initialization
	 * 
	 * @note
	 * See platform_init_early() for the rationale behind this function.
         */
	extern void platform_init_late(void);

    /**
     * Trigger a system reset
     * 
     * @note
     * This function never returns.
     

	// ==================================================================

	/**
	 * Add platform-event flags for retrieval via platform_evt_get()
	 * 
	 * This function is intended to be called from IRQ handlers.
	 *
	 * @note
	 * The set of event flags is completely application-defined. When
	 * defining flags, make sure no two flags use the same bit.
         */
	extern void platform_evt_add(int evt);
    
    /**
	 * For basic locomotion functions
         */
    extern void turn_right(void);
    extern void turn_left(void);
    extern void go_forward(void);
    extern void go_backward(void);
    extern void stop(void);
    
    /**
	 * For sensor checking
         */
    extern bool us_left(void);
    extern bool us_center(void);
    extern bool us_right(void);
    extern bool ir_left(void);
    extern bool ir_center(void);
    extern bool ir_center(void);
	
	/**
	 * Get available platform-event flags
	 *
	 * This function is intended to be called from within the main infinite
	 * loop.
	 *
	 * @note
	 * The set of event flags is completely application-defined. In
	 * addition, this function clears all added event flags so that they
	 * will not appear again until re-added via a call to
	 * platform_evt_add().
	 */
	extern int platform_evt_get(void);

	/// If set, the on-board button was pressed
#define PLATFORM_EVT_PB_PRESS	(0x00000001)
#define PLATFORM_TICK_MS	(10)
	
	
#define PLATFORM_MS_PER_SYSTICK	(10)

    

	// ==================================================================
#ifdef __cplusplus
}
#endif
#endif
#define PLATFORM_EVT_SW1_PRESS	(0x00000001)
    
extern uint32_t platform_tick_count(void);
	
	/// Get the difference between two ticks, considering overflow
extern uint32_t platform_tick_delta(uint32_t lhs, uint32_t rhs);

#define PLATFORM_TICK_MS	(10)
    
	
	// TODO: Other function prototypes may be placed here.

#ifdef __cplusplus
}
#endif

