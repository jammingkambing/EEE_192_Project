/* 
 * File:   main.c
 * Author: Jami
 *
 * Created on December 15, 2025, 10:10 AM
 */

#include "main.h"
#include "usart.h"
#include "init.h"
#include <xc.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

extern int trip;
extern int stepper;
volatile int idx_message;

int step_per;
int speed;
int setting = 0;
char setting_ptr[9];

static const char banner_msg[] =
"\033[2J\033[1;1H"
"+----------------------------------------------------------------+\r\n"
"| EEE 192: Electrical and Electronics Engineering Laboratory VI  |\r\n"
"|          Academic Year 2025-2026, Semester 2                   |\r\n"
"|                                                                |\r\n"
"| Robutt                                                         |\r\n"
"|                                                                |\r\n"
"| Author: Meows!                                                 |\r\n"
"|                                                                |\r\n"
"| Date:    2026                                                  |\r\n"
"+----------------------------------------------------------------+\r\n"
"| INSTRUCTIONS: Have fun!                                        |\r\n"
"+----------------------------------------------------------------+\r\n"
"\r\n"
"Direction: "	// Line 14
"\r\n"
"Speed: "
"\r\n"
"Mode: "
"\r\n"
"Ultrasonic sensor data:     LEFT:      CENTER:         RIGHT: "
"\r\n"
"Infrared sensor data:       LEFT:      CENTER:         RIGHT: ";

// Escape sequence prepended to the start of the sequence display
#define ESC_SEQ_BLINKMODE "\033[14;20H\033[0K"
#define ESC_SEQ_BRIGHTNESS "\033[15;20H\033[0K"
#define ESC_SEQ_MESSAGE "\033[16;20H\033[0K"
#define ESC_SEQ_US_LEFT "\033[17;34H\033[0K"
#define ESC_SEQ_IR_LEFT "\033[18;34H\033[0K"
#define ESC_SEQ_US_CENTER "\033[17;47H\033[0K"
#define ESC_SEQ_IR_CENTER "\033[18;47H\033[0K"
#define ESC_SEQ_US_CENTER "\033[17;47H\033[0K"
#define ESC_SEQ_IR_CENTER "\033[18;47H\033[0K"


// List of color schemes...
static const struct platform_ro_buf_desc color_schem[] = { // These are SGR colors
	{"\033[37;40m", 8},
    {"\033[36;40m", 8},
    {"\033[35;40m", 8},
    {"\033[33;40m", 8},
	{"\033[97;41m", 8},
	{"\033[92;40m", 8},
	{"\033[97;44m", 8},
    {"\033[30;47m", 8},
};
#define NR_COLOR_SCHEM (8)

//////////////////////////////////////////////////////////////////////////////

// Program state data
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

prog_state_t ps;



// Main-loop task handling data transmission
static void prog_loop_do_one_tx(prog_state_t *ps, int idx_message)
{
	if (platform_usart_cdc_tx_busy()) {
		ps->tx_flags |= 0x8000;
		return;
	}
	ps->tx_flags &= ~0x8000;
    if (ps->tx_flags == 0) {
		// Nothing to transmit
		return;
	}
	/*
	 * First check for the banner message. If it is set, all other updates
	 * are implied.
	 */
	ps->tx_nr_desc = 0;
	if ((ps->tx_flags & 0x0001) != 0) {
		// Full refresh implies all other messages
        
		ps->tx_flags |= 0x7FFB;
		if ((ps->tx_flags & 0x0004) != 0) {
			ps->idx_color_schem += 1;
			if (ps->idx_color_schem >= NR_COLOR_SCHEM) {
				ps->idx_color_schem = 0;
			}
		}
		ps->tx_desc[ps->tx_nr_desc] = color_schem[ps->idx_color_schem];
		ps->tx_nr_desc += 1;

		ps->tx_desc[ps->tx_nr_desc].buf = banner_msg;
		ps->tx_desc[ps->tx_nr_desc].len = sizeof(banner_msg)-1;
		ps->tx_nr_desc += 1;
        
       
   
	}
	if ((ps->tx_flags & 0x0002) != 0) {
        
        ps->tx_desc[ps->tx_nr_desc] = color_schem[idx_message];
		ps->tx_nr_desc += 1;
        
		ps->tx_desc[ps->tx_nr_desc].buf = ESC_SEQ_BLINKMODE;
		ps->tx_desc[ps->tx_nr_desc].len = sizeof(ESC_SEQ_BLINKMODE)-1;
		ps->tx_nr_desc += 1;
        
		switch (setting) {
            case 0:
            ps->tx_desc[ps->tx_nr_desc].buf = "STOP";
            ps->tx_desc[ps->tx_nr_desc].len = 4;
            break;
            case 1:
            ps->tx_desc[ps->tx_nr_desc].buf = "FORWARD";
            ps->tx_desc[ps->tx_nr_desc].len = 7;
            break;
            case 2:
            ps->tx_desc[ps->tx_nr_desc].buf = "BACKWARD";
            ps->tx_desc[ps->tx_nr_desc].len = 8;
            break;
            case 3:
            ps->tx_desc[ps->tx_nr_desc].buf = "RIGHT";
            ps->tx_desc[ps->tx_nr_desc].len = 5;
            break;
            case 4:
            ps->tx_desc[ps->tx_nr_desc].buf = "LEFT";
            ps->tx_desc[ps->tx_nr_desc].len = 4;
            break;
        }
        
        ps->tx_nr_desc += 1;
        
        ps->tx_desc[ps->tx_nr_desc].buf = ESC_SEQ_BRIGHTNESS;
		ps->tx_desc[ps->tx_nr_desc].len = sizeof(ESC_SEQ_BRIGHTNESS)-1;
        ps->tx_nr_desc += 1;

        switch (speed) {
            case 0:
            ps->tx_desc[ps->tx_nr_desc].buf = "okay lang";
            ps->tx_desc[ps->tx_nr_desc].len = 9;
            break;
            case 1:
            ps->tx_desc[ps->tx_nr_desc].buf = "medyo fast";
            ps->tx_desc[ps->tx_nr_desc].len = 10;
            break;
            case 2:
            ps->tx_desc[ps->tx_nr_desc].buf = "SKREEEE";
            ps->tx_desc[ps->tx_nr_desc].len = 7;
            break;
            
        }
        
        
        ps->tx_nr_desc += 1;
        
        ps->tx_desc[ps->tx_nr_desc].buf = ESC_SEQ_MESSAGE;
		ps->tx_desc[ps->tx_nr_desc].len = sizeof(ESC_SEQ_MESSAGE)-1;
        ps->tx_nr_desc += 1;
        
        switch (idx_message) {
            case 0:
            ps->tx_desc[ps->tx_nr_desc].buf = "REMOTE";
            ps->tx_desc[ps->tx_nr_desc].len = 6;
            break;
            case 1:
            ps->tx_desc[ps->tx_nr_desc].buf = "AUTONOMOUS";
            ps->tx_desc[ps->tx_nr_desc].len = 9;
            break;
            
        }
        ps->tx_nr_desc += 1;

	}

	// Enqueue them for transmission
	if (ps->tx_nr_desc == 0) {
		// Nothing to transmit
		return;
	}
	platform_usart_cdc_send_async(&ps->tx_desc[0], ps->tx_nr_desc);
	ps->tx_flags = 0x8000;
}

// Main-loop task handling data reception
static void prog_loop_do_one_rx(prog_state_t *ps)
{
	// Process the reception-side.
	switch (ps->state_id) {
	case 0:
		// IDLE; Start a new reception, if not done so
		if (platform_usart_cdc_recv_async(ps->rx_buf, sizeof(ps->rx_buf))) {
			ps->state_id += 1;
		}
		break;
	case 1:
		// Waiting to receive a message
		if (platform_usart_cdc_recv_get(&ps->rx_info)) {
			ps->state_id += 1;
			// Fall through
		} else {
			// No message received yet
            stop();
            setting = 0;
			break;
		}
	case 2:
		// Waiting for the transmission to complete
		if ((ps->tx_flags & 0x8000) != 0) {
			// Still busy...
			break;
		}   
        
        ps->tx_buf_len = 0;   
         
        ps->tx_flags |= 0x0002;
		ps->state_id += 1;
        
        //arrows
        if (ps->rx_info.len == 3 && ps->rx_info.buf[1] == 0x5B) {
            if (ps->rx_info.buf[2] == 0x41) {
              setting = 1;
            }
            else if (ps->rx_info.buf[2] == 0x42) { 
              setting = 2;
            }
            else if (ps->rx_info.buf[2] == 0x43) {
              setting = 3;
            }
            else if (ps->rx_info.buf[2] == 0x44) {
              setting = 4;
            }
            else {
                setting = 0;
            }
        }
		else if (ps->rx_info.len > 1 && ps->rx_info.buf[0] == '\033') {
			/*
			 * Escape sequence
			 * 
			 * Unlike Python or similar, "a" == "b" does not work
			 * as expected; this always returns false, since '=='
			 * compares address values, but the C standard requires
			 * variables to have unique addresses.
			 * 
			 * Instead, memcmp() or strcmp() should be used, which
			 * return 0 if the blobs are identical.
			 */
			if (ps->rx_info.len == 5 &&
			    !memcmp(ps->rx_info.buf+1, "[15~", 4)) {
				// Refresh (no color change)
				ps->tx_flags |= 0x0001;
			}
		}
        else {
            setting = 0;
        }
		break;
	case 3:
		// Waiting for the message to be transmitted
		if ((ps->tx_flags & 0x8002) != 0) {
			break;
		}
		ps->state_id = 0;
		break;
	default:
		/*
		 * Unrecognized state
		 * 
		 * Effectively, the state of the program is undefined; often,
		 * a forced reset is the only sane option.
		 */
		platform_do_sys_reset();
	}
}


int main(void) {
    
    int a_speed, b_speed = 0;
    int line_left, line_center, line_right = 0;
    
    struct {
		unsigned int sweep;
	} tick_ctrs;
    tick_ctrs.sweep = 0;
    unsigned int ts_delta, ts_curr;
    
    platform_init_early();
    platform_usart_cdc_init();
    platform_init_late();
    
    ps.tx_flags = 0x0001;
    
    idx_message = 0;
    
    for (;;) {
        
       if ((PORT_SEC_REGS->GROUP[0].PORT_IN & (1 << 9)) == 1) {
           line_right = 1;
       }
       else {
           line_right = 0;
       }
       
       if ((PORT_SEC_REGS->GROUP[0].PORT_IN & (1 << 10)) == 1) {
           line_center = 1;
       }
       else {
           line_center = 0;
       }
       
       if ((PORT_SEC_REGS->GROUP[0].PORT_IN & (1 << 11)) == 1) {
           line_right = 1;
       }
       else {
           line_right = 0;
       }
      
       ts_curr = platform_systick_count();
       ts_delta = platform_tick_delta(ts_curr, tick_ctrs.sweep);
       
       prog_loop_do_one_tx(&ps, idx_message);
       prog_loop_do_one_rx(&ps);
       
        if (ts_delta >= (20/PLATFORM_TICK_MS)) {
//          // At least 20 ms have elapsed
            tick_ctrs.sweep = ts_curr;
        }
        
       switch (setting) {
           case 0:
               stop();
               break;
           case 1:
               go_forward();
               a_speed = 50;
               b_speed = 35;
               break;
           case 2: 
               go_backward();
               a_speed = 50;
               b_speed = 35;
               break;
           case 3: 
               turn_right();
               a_speed = 50;
               b_speed = 50;
               break;
           case 4: 
               turn_left();
               a_speed = 50;
               b_speed = 50;
               break;
       }
       
       // I'm gonna have to figure out the logic for this... disgusting
       if ((TCC3_REGS->TCC_SYNCBUSY & 0x00000080) == 0) {
                TCC3_REGS->TCC_CCBUF[1] = a_speed;
                TCC3_REGS->TCC_CCBUF[0] = b_speed;
	}
        
    }
}
