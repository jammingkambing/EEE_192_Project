/*
 * platform.c
 *
 * Initial version:  EEE 158 AY2024-25 Sem 1 Handlers
 * Modifications by: DE VILLA, Alberto
 *
 * Platform-interface functionality, to avoid cluttering main.c with
 * initialization logic
 */

#include "main.h"
#include "init.h"
#include "usart.h"

#include <xc.h>
#include <stdint.h>

// Defined in irq_handlers.c
extern void platform_do_enable_interrupts(void);

// Defined elsewhere in this file
static void do_raise_perf_level(void);
static void PORT_init_early(void);
static void EIC_init_early(void);
static void EIC_init_late(void);
static void TC_init_early(void);
static void TC_init_late(void);
static void ADC_init(void);

// ==========================================================================

// Initialize the platform
void platform_init_early(void)
{
	// MUST BE CALLED FIRST
	do_raise_perf_level();

	// Other early initialization
	PORT_init_early();
	EIC_init_early();
    TC_init_early();

	/*
	 * Configure SysTick
	 *
	 * Since we have configured our CPU clock to 24 MHz, one tick will now
	 * correspond to
	 *
	 * (24e6 Hz)^(-1) --> approx. 41.666667 ns
	 */
	SysTick->LOAD = 3750; 	// Once every 10ms

	/*
	 * Enabling of timers, SysTick, and interrupts have been separated
	 * into another function. If main() still has other tasks to do, it
	 * must be done before enabling them.
	 */
	return;
}

// Enable interrupts; MUST BE CALLED LAST
void platform_init_late(void)
{
	/*
	 * Enable the interrupts + TC0; MUST BE LAST prior to entering the
	 * main infinite loop
	 */
    ADC_init();
	EIC_init_late();
    TC_init_late();
    
    NVIC_SetPriority(SERCOM3_0_IRQn, 3);		// usart.c
	NVIC_SetPriority(SERCOM3_2_IRQn, 3);		// usart.c
	

	NVIC_EnableIRQ(SERCOM3_0_IRQn);
	NVIC_EnableIRQ(SERCOM3_2_IRQn);
	return;
}

// ==========================================================================

/*
 * Change the CPU clock from its default of 4 MHz at reset, to 24 MHz
 *
 * As this fundamentally affects operation of the microcontroller, call this
 * as early as possible.
 *
 * This is necessary because the TC/TCC peripheral must run slower than the CPU
 * core clock, yet the former needs to run relatively fast in PWM/input-capture
 * setups (cue: sampling theorem).
 */
void do_raise_perf_level(void)
{
	uint32_t tmp_reg = 0;

	/*
	 * The chip starts in PL0, which emphasizes energy efficiency over
	 * performance. However, we need the latter for the clock frequency
	 * we will be using (~24 MHz); hence, switch to PL2 before continuing.
	 */
	PM_REGS->PM_INTFLAG = 0x01;
	PM_REGS->PM_PLCFG = 0x02;
	while ((PM_REGS->PM_INTFLAG & 0x01) == 0) {
		asm("nop");
	}
	PM_REGS->PM_INTFLAG = 0x01;

	/*
	 * Before we power up the 48MHz DFPLL, we need to ensure that all
	 * electrical characteristics remain respected after the change.
	 *
	 * For the NVM controller (which handles Flash memory, as well as
	 * reads of factory-calibration data; and is clocked at CPU frequency),
	 * the default is to have zero wait states. Unfortunately, at PL2 the
	 * resulting maximum allowable frequency is 11 MHz, which is below our
	 * target CPU frequency of 24 MHz. To rectify this, add enough wait
	 * states; here, 3 is used to provide some margin. (The lowest
	 * allowable in this context is 2.)
	 */
	NVMCTRL_SEC_REGS->NVMCTRL_CTRLB = (3 << 1) ;

	/*
	 * Power up the 48MHz DFPLL.
	 *
	 * On the Curiosity Nano Board, VDDPLL has a 1.1uF capacitance
	 * connected in parallel. Assuming a ~20% error, we have
	 * STARTUP >= (1.32uF)/(1uF) = 1.32; as this is not an integer, choose
	 * the next HIGHER value.
	 */
	SUPC_REGS->SUPC_VREGPLL = 0x00000202;
	while ((SUPC_REGS->SUPC_STATUS & (1 << 18)) == 0) {
		asm("nop");
	}

	/*
	 * Configure the 48MHz DFPLL.
	 *
	 * Start with disabling ONDEMAND...
	 */
	OSCCTRL_REGS->OSCCTRL_DFLLCTRL = 0x0000;
	while ((OSCCTRL_REGS->OSCCTRL_STATUS & (1 << 24)) == 0) {
		asm("nop");
	}

	/*
	 * ... then writing the calibration values (which MUST be done as a
	 * single write, hence the use of a temporary variable)...
	 *
	 * NOTE: The "FINE" value is arbitrary.
	 */
	tmp_reg = SW_CALIB_FUSES_REGS->FUSES_SW_CALIB_WORD_0 & 0x7E000000;
	asm("nop");
	tmp_reg >>= 15;
	tmp_reg |= ((512 << 0) & 0x000003ff);
	OSCCTRL_REGS->OSCCTRL_DFLLVAL = tmp_reg;
	while ((OSCCTRL_REGS->OSCCTRL_STATUS & (1 << 24)) == 0) {
		asm("nop");
	}

	/*
	 * ... then enabling.
	 *
	 * Because DFLL48M will be used as the CPU clock source
	 * (via GCLK_GEN0), ONDEMAND cannot be enabled for this oscillator.
	 */
	OSCCTRL_REGS->OSCCTRL_DFLLCTRL |= 0x0002;
	while ((OSCCTRL_REGS->OSCCTRL_STATUS & (1 << 24)) == 0) {
		asm("nop");
	}

	/*
	 * Because GCLK_GEN0 will be reconfigured to 24 MHz (the new CPU clock
	 * frequency), a second channel (here, GCLK_GEN2) is configured to
	 * take GCLK_GEN0's place as a peripheral clock source for
	 * slow/medium-speed peripherals.
	 *
	 * NOTE: GCLK_GEN1 is a special instance with additional features. As
	 *       said features (with their attendant complexity) are not
	 *       needed, use a normal instance.
	 *
	 * NOTE: GENCTRL must be written as a single operation; thus, to both
	 *       set and clear bits a read-modify-write operation with a
	 *       temporary variable is required (cue PMUX in PORT).
	 */
	GCLK_REGS->GCLK_GENCTRL[2] = 0x00010105;
	while ((GCLK_REGS->GCLK_SYNCBUSY & (1 << 4)) != 0) {
		asm("nop");
	}

	/*
	 * Switch over GCLK_GEN0 from OSC16M to DFLL48M, with DIV=2 to get
	 * 24 MHz. (48 MHz is unnecessarily too fast...)
	 */
	GCLK_REGS->GCLK_GENCTRL[0] = 0x00020107;
	while ((GCLK_REGS->GCLK_SYNCBUSY & (1 << 2)) != 0) {
		asm("nop");
	}

	// Done. We're now at 24 MHz.
	return;
}

// Configure port I/O here; do not enable PMUXEN yet
void PORT_init_early(void)
{
    // PA00: Input Latch SW3
    PORT_SEC_REGS->GROUP[0].PORT_DIRCLR = (1 << 0);
	PORT_SEC_REGS->GROUP[0].PORT_PINCFG[0] = 0x02;
    
    // PA01: Input Latch SW4
    PORT_SEC_REGS->GROUP[0].PORT_DIRCLR = (1 << 1);
	PORT_SEC_REGS->GROUP[0].PORT_PINCFG[1] = 0x02;
    
    // PA02: Active-HI Output
	PORT_SEC_REGS->GROUP[0].PORT_OUTCLR = (1 << 2);
	PORT_SEC_REGS->GROUP[0].PORT_DIRSET = (1 << 2);
    
    // PA03: Active-HI Output, RED LED
	PORT_SEC_REGS->GROUP[0].PORT_OUTCLR = (1 << 3);
	PORT_SEC_REGS->GROUP[0].PORT_DIRSET = (1 << 3);
    
    // PA06: Active-HI Output
	PORT_SEC_REGS->GROUP[0].PORT_OUTCLR = (1 << 6);
	PORT_SEC_REGS->GROUP[0].PORT_DIRSET = (1 << 6);
    
    // PA07: Active-HI Output
	PORT_SEC_REGS->GROUP[0].PORT_OUTCLR = (1 << 7);
	PORT_SEC_REGS->GROUP[0].PORT_DIRSET = (1 << 7);
    
	// PA20: Active-HI Output, DRV.STEP
    PORT_SEC_REGS->GROUP[0].PORT_OUTCLR = (1 << 20);
	PORT_SEC_REGS->GROUP[0].PORT_DIRSET = (1 << 20);
    
    // PA14: Active-HI Output, DRV.DIR
    PORT_SEC_REGS->GROUP[0].PORT_OUTCLR = (1 << 14);
	PORT_SEC_REGS->GROUP[0].PORT_DIRSET = (1 << 14);
    
    // PA15: Active-HI LED
    PORT_SEC_REGS->GROUP[0].PORT_OUTCLR = (1 << 15);
	PORT_SEC_REGS->GROUP[0].PORT_DIRSET = (1 << 15);
    
    // PA15: Active-HI Output
    PORT_SEC_REGS->GROUP[0].PORT_OUTCLR = (1 << 19);
	PORT_SEC_REGS->GROUP[0].PORT_DIRSET = (1 << 19);
    
    // PA23: Active-LO Push Button
    PORT_SEC_REGS->GROUP[0].PORT_DIRCLR = (1 << 23);
	PORT_SEC_REGS->GROUP[0].PORT_PINCFG[23] = 0x02;
    
    // PB03: Active-HI Output
    PORT_SEC_REGS->GROUP[1].PORT_OUTCLR = (1 << 3);
	PORT_SEC_REGS->GROUP[1].PORT_DIRSET = (1 << 3);
    
    // PB22: Active-LO SYS.TRIP (overline)
    PORT_SEC_REGS->GROUP[1].PORT_DIRCLR = (1 << 22);
	PORT_SEC_REGS->GROUP[0].PORT_PINCFG[22] = 0x02;
    
    // PB23: Active-HI Output, DRV.EN
    PORT_SEC_REGS->GROUP[1].PORT_OUTCLR = (1 << 23);
	PORT_SEC_REGS->GROUP[1].PORT_DIRSET = (1 << 23);
    
    
	return;

}

// Configure EIC here; do not enable it yet
void EIC_init_early(void)
{
	uint8_t scratch = 0;

	/*
	 * =============================================================
	 * Configure GCLK to enable EIC's filtering + debouncing
	 *
	 * This assignment selects GCLK2, then enables GCLK for EIC.
	 * =============================================================
	 */
	GCLK_REGS->GCLK_PCHCTRL[4] = 0x00000042;
	while ((GCLK_REGS->GCLK_PCHCTRL[4] & (1 << 6)) == 0) {
		// Wait for synchronization
		asm("nop");
	}

	/*
	 * =============================================================
	 * Configure EIC itself. First tasks should be setting the SWRST bit
	 * to 1, then waiting for the reset to complete (cue SYNCBUSY).
	 *
	 * ENABLE should be set last.
	 * =============================================================
	 */
	EIC_SEC_REGS->EIC_CTRLA = 0x01; // Reset
	while ((EIC_SEC_REGS->EIC_SYNCBUSY & (1 << 0)) != 0) {
		// Wait for synchronization
		asm("nop");
	}

	/*
	 * PA23 maps to EXTINT_2, according to the pinout of the
	 * PIC32CM5164LS00048 device package.
	 *
	 * Configure EXTINT_2 for falling-edge-only operation.
	 *
	 * NOTE: Do not enable EIC yet. Once it is, most configurations will be
	 *       locked from further updates.
	 */
	// NOTE: CONFIG1 has Channels 8..15
	EIC_SEC_REGS->EIC_CONFIG0 = 0x02000AAA;
	EIC_SEC_REGS->EIC_DEBOUNCEN |= 3;	// Tied to mech button
	EIC_SEC_REGS->EIC_DPRESCALER = 0x0000000E;	// Mech input only
	EIC_SEC_REGS->EIC_INTENSET = 71;		// Unmask the interrupt

	/*
	 * Reroute PA23 from GPIO to EIC
	 *
	 * NOTE: PORT_PMUX[n] configures both Channel 2n and Channel (2n+1),
	 *       making direct bitmask operations risky. Hence, a
	 *       read-modify-write operation is needed.
	 *
	 * For PA23 (odd), 2n+1=23 yields n=11. Furthermore, EXTINT[2] is on
	 * Function A (cue: device pinout).
	 */
	scratch = PORT_SEC_REGS->GROUP[0].PORT_PMUX[11];
	scratch &= ~(0xF0);	// Odd --> high 4 bits; Even --> low 4 bits
	scratch |=  (0x00);	// Function A for odd-numbered channel
	PORT_SEC_REGS->GROUP[0].PORT_PMUX[11] = scratch;
	PORT_SEC_REGS->GROUP[0].PORT_PINCFG[23] |= 0x01; // Enable PMUXEN
    
    // Need to do the same thing for PB22, which maps to EXTINT[6], and configure for falling edge
    
    scratch = PORT_SEC_REGS->GROUP[1].PORT_PMUX[11];
	scratch &= ~(0x0F);	// Odd --> high 4 bits; Even --> low 4 bits
	scratch |=  (0x00);	// Function A for odd-numbered channel
	PORT_SEC_REGS->GROUP[1].PORT_PMUX[11] = scratch;
	PORT_SEC_REGS->GROUP[1].PORT_PINCFG[22] |= 0x01; // Enable PMUXEN
    
    //PA00
    
    scratch = PORT_SEC_REGS->GROUP[0].PORT_PMUX[0];
	scratch &= ~(0x0F);	// Odd --> high 4 bits; Even --> low 4 bits
	scratch |=  (0x00);	// Function A for odd-numbered channel
	PORT_SEC_REGS->GROUP[0].PORT_PMUX[0] = scratch;
	PORT_SEC_REGS->GROUP[0].PORT_PINCFG[0] |= 0x01; // Enable PMUXEN
    
    //PA01
    
    scratch = PORT_SEC_REGS->GROUP[0].PORT_PMUX[0];
	scratch &= ~(0xF0);	// Odd --> high 4 bits; Even --> low 4 bits
	scratch |=  (0x00);	// Function A for odd-numbered channel
	PORT_SEC_REGS->GROUP[0].PORT_PMUX[0] = scratch;
	PORT_SEC_REGS->GROUP[0].PORT_PINCFG[1] |= 0x01; // Enable PMUXEN
}

void TC_init_early(void) {
    /*
	 * =============================================================
	 * Configure GCLK
	 *
	 * Unlike EIC where GCLK is only needed for filtering + debouncing
	 * functionality, GCLK is unconditionally needed for TC/TCC as it
	 * serves as the module's clock source.'
	 *
	 * This assignment selects GCLK2, then enables GCLK for TC0.
	 * (Why this TC instance?)
	 * =============================================================
	 */
	GCLK_REGS->GCLK_PCHCTRL[23] = 0x00000042;
	while ((GCLK_REGS->GCLK_PCHCTRL[23] & (1 << 6)) == 0) {
		// Wait for synchronization
		asm("nop");
	}

	/*
	 * =============================================================
	 * Configure TC itself. First task should be a soft-reset via
	 * the SWRST bit; then configuration of 8/16/32-bit mode.
	 *
	 * For the PIC32CM series, TC is accessed differently depending on
	 * whether the instance has an 8-bit, 16-bit, or 32-bit counter.
	 * Upon reset, the instance starts in 16-bit mode (hence "COUNT16").
	 *
	 * ENABLE should be set last.
	 * =============================================================
	 */
	TC1_REGS->COUNT16.TC_CTRLA = 0x00000001;
	while ((TC1_REGS->COUNT16.TC_SYNCBUSY & (1 << 0)) != 0) {
		// Wait for synchronization
		asm("nop");
	}
	// TC0_REGS->COUNT16.TC_CTRLA = 0x00000000; // 16-bit mode

	/*
	 * The human eye can see up to 30 frames per second ("fps", 30 Hz);
	 * some might even go to 60 fps (60 Hz). To cover all bases, let's'
	 * use 500 Hz.
	 *
	 * WARNING: PWM dimming is inherently a strobe-type operation. If you
	 *          are sensitive to epilepsy due to fast-changing strobe
	 *          lighting, please exercise extreme caution when setting the
	 *          PWM frequency.
	 *
	 * To be able to adjust the duty cycle in 1% increments, PER must be
	 *
	 * PER + 1 = 100 --> PER = 99
	 *
	 * To be able to have f_{pwm}=500 while being able to do the preceding
	 * adjustments, the minimum prescaled GCLK frequency must be
	 *
	 * GCLK_TC0 / (PSC) = (f_{pwm})*(PER+1)
	 *
	 * For a 4MHz GCLK_TC0, we get PSC=256.
	 * Consequently, the real PWM frequency will be
	 * 
	 * (4e6) / (64*(99+1)) = 15625 Hz
	 */
	TC1_REGS->COUNT16.TC_CTRLA |= 0x00000400;

	/*
	 * Select output-compare sub-mode; classic PWM corresponds to single-
	 * slope PWM.
	 *
	 * At reset, the TC instance starts in output-compare mode. (How is
	 * this so?)
	 */
	TC1_REGS->COUNT16.TC_WAVE = 0x02;

	//We want a 1000 Hz freq

	// Start at 0% duty cycle (aka. off)
    TC1_REGS->COUNT16.TC_PER = 249;
	TC1_REGS->COUNT16.TC_CC[0] = 0;
	while ((TC1_REGS->COUNT16.TC_SYNCBUSY & 0x000000A0) != 0) {
		asm("nop");
	}

	/*
	 * Enable double-buffering.
	 *
	 * (It is enabled upon reset.)
	 */

	/*
	 * Reroute PA20 from GPIO to TC1, noting the need for a read-modify-
	 * write operation.
	 *
	 * NOTE: When a pin is used for PWM, DRVSTR should be set to help
	 *       overcome parasitic capacitance.
	 *
	 * For PA20 (odd), 2n=20 yields n=10. Furthermore, TC1/WO[1] is on
	 * Function E (cue: device pinout).
	 */
    
    uint8_t is_on;
	is_on = PORT_SEC_REGS->GROUP[0].PORT_PMUX[10];
	is_on &= ~(0x0F);	// Odd --> high 4 bits; Even --> low 4 bits
	is_on |=  (0x04);	// Function E for odd-numbered channel
	PORT_SEC_REGS->GROUP[0].PORT_PMUX[10] = is_on;
	PORT_SEC_REGS->GROUP[0].PORT_PINCFG[20] |= 0x41; // Enable PMUXEN + DRVSTR
}

void TC_init_late(void) {
    TC1_REGS->COUNT16.TC_CTRLA |= 0x00000002;
	while ((TC1_REGS->COUNT16.TC_SYNCBUSY & 0x00000002) != 0) {
		asm("nop");
	}
}
// Late initialization for EIC
void EIC_init_late(void)
{
	EIC_SEC_REGS->EIC_CTRLA = 0x02;			// Enable EIC
	while ((EIC_SEC_REGS->EIC_SYNCBUSY & (1 << 1)) != 0) {
		// Wait for synchronization
		asm("nop");
	}
	EIC_SEC_REGS->EIC_INTFLAG = 0x0000ffff;
    
    NVIC_SetPriority(EIC_EXTINT_0_IRQn, 2);
	NVIC_EnableIRQ(EIC_EXTINT_0_IRQn);
    
    NVIC_SetPriority(EIC_EXTINT_1_IRQn,3);
	NVIC_EnableIRQ(EIC_EXTINT_1_IRQn);
    
    NVIC_SetPriority(EIC_EXTINT_2_IRQn, 3);
	NVIC_EnableIRQ(EIC_EXTINT_2_IRQn);
    
    NVIC_SetPriority(EIC_EXTINT_6_IRQn,3);
	NVIC_EnableIRQ(EIC_EXTINT_6_IRQn);

	// SysTick: select CPU clock source and enable (w/ interrupts)
	SysTick->CTRL = 0x00000007;

	return;
}

void ADC_init(void)
{
    int scratch = 0;
    
    scratch = PORT_SEC_REGS->GROUP[1].PORT_PMUX[1];
	scratch &= ~(0x0F);	// Odd --> high 4 bits; Even --> low 4 bits
	scratch |=  (0x01);	// Function A for even-numbered channel
	PORT_SEC_REGS->GROUP[1].PORT_PMUX[1] = scratch;
    PORT_SEC_REGS->GROUP[1].PORT_PINCFG[1] |= 0x01; // Enable PMUXEN
    
    GCLK_REGS->GCLK_PCHCTRL[28] = 0x00000042;
    while ((GCLK_REGS->GCLK_PCHCTRL[28] & (1 << 6)) == 0) {
        asm("nop");
    }
    
    ADC_REGS->ADC_CTRLA |= 1; // SWRST
    while ((ADC_REGS->ADC_SYNCBUSY & 1) != 0) {
		// Wait for synchronization
		asm("nop");
	 }
    
    ADC_REGS->ADC_REFCTRL |= 5; // SET AVDD REFERENCE
    ADC_REGS->ADC_CTRLC |= (1 << 5); // 10-BIT
    ADC_REGS->ADC_CTRLB = 0x04; // Have prescaler set to 32
	
	/*
	 * The external analog input is on Channel 10 (aka. PB02, potentiometer); route this
	 * to the (+) input. The (-) input, meanwhile, should be tied to AVSS.
	 * 
	 * (Why is this so?)
	 */
	ADC_REGS->ADC_INPUTCTRL |= 10; // Set positive input to AIN[10]
    ADC_REGS->ADC_INPUTCTRL |= 6144;// 0x18 = AVSS, MUXNEG is in bits [12:8]
	
	// Write the calibration value to the ADC, prior to enabling it.
	uint32_t t1 = SW_CALIB_FUSES_REGS->FUSES_SW_CALIB_WORD_0;
	uint16_t t2 = 0x0000;
	
    // Note that currently ADC BIASREBUF [2:0], ADC BIASCOMP [5:3]
    // Set calibration value to ADC BIASREFBUF [10:8], ADC BIASCOMP [2:0]
    // The above bit positions are from Chap 44.7.22
    
    
    t2  |= (uint16_t)((t1 & 0x00000007) << 8);
    t2  |= (uint16_t)((t1 & 0x00000038) >> 3);
    
    ADC_REGS->ADC_CALIB = t2;
	
    
    ADC_REGS->ADC_CTRLA |= 2; // ENABLE
    while ((ADC_REGS->ADC_SYNCBUSY & 2) != 0) {
        asm("nop");
    }
    ADC_REGS->ADC_CTRLC |= (1 << 2); //SET TO FREERUNNING
    ADC_REGS->ADC_SWTRIG = 2; // Start first conversion
    
	return;
}

void __attribute__((noreturn)) platform_do_sys_reset(void)
{
	uint32_t sc = SCB->AIRCR;
	
	/*
	 * Set up the correct value for SCB.AIRCR; this must be written as a
	 * single operation.
	 * 
	 * See the Arm-v8M Architecture Reference Manual for more details.
	 */
	sc &= ~(0x00000004);
	sc ^=  (0xFFFF0004);
	
	/*
	 * Once this statement starts executing, control does not return to
	 * this function.
	 */
	SCB->AIRCR = sc;
	
	/*
	 * Just a formality, but necessary to ensure this function never
	 * returns to its caller.
	 */
	while (1) {
		asm("nop");
	}
}

// ==========================================================================




