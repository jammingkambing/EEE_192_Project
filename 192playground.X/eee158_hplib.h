/**
 * @file eee158_hplib.h
 *
 * Initial version:  EEE 158 AY2025-26 Sem 1 Handlers
 *
 * Prototypes for functions mostly implemented by the handlers as a "black box"
 * library
 * 
 * NOTE: No modifications should be made to this file.
 */

/*
 * The outermost #if-#endif block makes sure that if this header file is
 * included multiple times (virtually always the case in production code), we
 * don't get multiple-definition errors from the compiler.
 *
 * (C/C++ Standard, "One-definition Rule" [ODR])
 */
#ifndef EEE158_HPLIB_H
#define EEE158_HPLIB_H

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
	 * Application-defined function to configure and enable other
	 * peripherals.
	 *
	 * If no function with this name is defined, no additional items are
	 * configured.
	 *
	 * @note
	 * This function is called by platform_init_early() just prior to
	 * returning. As such, certain info regarding the platform state is
	 * already applicable upon entry into this function.
	 */
	extern void platform_do_app_init(void);

	/**
	 * Application-defined function to configure and enable NVIC IRQ lines,
	 * as well as enable other peripherals.
	 * 
	 * If no function with this name is defined, no additional items are
	 * configured.
	 * 
	 * @note
	 * This function is called by platform_init_late() just prior to
	 * enabling its own interrupts.
	 */
	extern void platform_do_app_init_late(void);
	
	// ==================================================================

	/**
	 * Add platform-event flags for retrieval via platform_evt_get()
	 * 
	 * This function is intended to be called from IRQ handlers.
	 *
	 * @note
	 * The set of event flags is completely application-defined.
         */
	extern void platform_evt_add(int evt);
	
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
	
	// ==================================================================

	/// Get the current number of SysTick occurrences, as a raw value
	extern uint32_t platform_systick_count(void);
	
	/// Get the difference between two ticks, considering overflow
	extern uint32_t platform_tick_delta(uint32_t lhs, uint32_t rhs);
	
	/// Number of milliseconds corresponding to one SysTick occurrence
#define PLATFORM_MS_PER_SYSTICK	(10)

	// ==================================================================
#ifdef __cplusplus
}
#endif
#endif
