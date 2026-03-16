/*
 * interrupts.c
 *
 * Initial version:  EEE 158 AY2025-26 Sem 1 Handlers
 * Modifications by: <Insert name, section, student number here>
 *
 * Interrupt-handling functionality, separated to minimize clutter in main.c
 */

#include "main.h"
#include "init.h"
#include "usart.h"

#include <xc.h>
#include <stdint.h>
#include <limits.h>

#define PLATFORM_EVT_ALL (PLATFORM_EVT_SW1_PRESS)

volatile int line_left = 0;
volatile int line_center = 0;
volatile int line_right = 0;
static volatile int evt_flags = 0;
static volatile int tick_ctr = 0;
typedef struct prog_state_type
{
	/**
	 * Flags for this program:
	 *
	 * 0x0001 = Full refresh pending
	 * 0x0002 = Message update pending (implied by full refresh)
	 * 0x0004 = Change colors (useful only with 0x0001)
	 * 0x8000 = Transmission on-going
	 */
	uint16_t tx_flags;
	
	/*
	 * State ID
	 * 
	 *   0 = Idle (initial)
	 * 0.5 = Initiate reception
	 *   1 = Waiting for input
	 *   2 = Waiting for any current transmission to complete
	 * 2.5 = Generate output and enqueue it for transmission
	 *   3 = Waiting for the output buffer to be fully transmitted
	 */
	unsigned int state_id;
	
	// Transmit stuff
	struct platform_ro_buf_desc tx_desc[12];
	uint16_t tx_nr_desc;
	char	tx_buf[60];
	unsigned int tx_buf_len;
	unsigned int idx_color_schem;
    
	
	// Receiver stuff
	struct platform_usart_recv_data_type rx_info;
	char rx_buf[16];
} prog_state_t;

extern prog_state_t ps;

// Tick count
static volatile uint32_t ctr_nres = 0;
uint32_t platform_systick_count(void)
{
	uint32_t res;

	/*
	 * This loop guards against the possibility of returning inconsistent
	 * data to the caller -- in the unlikely case that the copy was
	 * interrupted, it is retried.
	 */
	do {
		res = ctr_nres;
		asm("nop");
	} while (res != ctr_nres);

	return res;
}

void __attribute__((interrupt)) SysTick_Handler(void)
{
    static int div_n = 0;

	platform_internal_usart_cdc_systick_handler();
	++div_n;
	while (div_n >= 64) {
		div_n -= 64;
		++ctr_nres;
	}
	// Periodic semaphore tasks here
	++tick_ctr;
	return;
}

//SW4
void __attribute__((interrupt)) EIC_EXTINT_0_Handler(void)
{
	// ISR body here. Keep it short and simple!
	if ((EIC_SEC_REGS->EIC_INTFLAG & (1 << 0)) != 0) {

		EIC_SEC_REGS->EIC_INTFLAG |= (1 << 0);
	}
}

//SW3
void __attribute__((interrupt)) EIC_EXTINT_1_Handler(void)
{
	// ISR body here. Keep it short and simple!
	if ((EIC_SEC_REGS->EIC_INTFLAG & (1 << 1)) != 0) {

		EIC_SEC_REGS->EIC_INTFLAG |= (1 << 1);
	}
}

void __attribute__((interrupt)) EIC_EXTINT_2_Handler(void)
{
	// ISR body here. Keep it short and simple!
	if ((EIC_SEC_REGS->EIC_INTFLAG & (1 << 2)) != 0) {

		EIC_SEC_REGS->EIC_INTFLAG |= (1 << 2);
	}
}

void __attribute__((interrupt)) EIC_EXTINT_6_Handler(void)
{
	// ISR body here. Keep it short and simple!
	if ((EIC_SEC_REGS->EIC_INTFLAG & (1 << 6)) != 0) {
		// EXTINT_2 was signaled; configured for falling-edge only
        ps.tx_flags |= 0x0002;
		// EDIT MO TOH
		EIC_SEC_REGS->EIC_INTFLAG |= (1 << 6);
	}
}

/// Tick difference, aware of wrap-arounds
uint32_t platform_tick_delta(uint32_t lhs, uint32_t rhs)
{
	uint32_t res;
	
	if (rhs <= lhs) {
		// Normal case
		res = lhs - rhs;
	} else {
		// Wrap-around case
		res  = rhs - lhs;
		asm("nop");
		res -= (UINT32_MAX - 1);
	}
	return res;
}

uint32_t platform_tick_count(void)
{
	uint32_t res;
	
	/*
	 * This loop guards against the possibility of returning inconsistent
	 * data to the caller -- in the unlikely case that the copy was
	 * interrupted, it is retried.
	 */
	do {
		res = tick_ctr;
		asm("nop");
	} while (res != tick_ctr);
	
	return res;
}

