//******************************************************************************
//! @file $RCSfile: uart_gcc_drv.c,v $
//!
//! Copyright (c) 2007 Atmel.
//!
//! Use of this program is subject to Atmel's End User License Agreement.
//! Please read file license.txt for copyright notice.
//!
//! @brief UART driver file working with GCC library.
//!        This file contains the low level functions (drivers) of:
//!             - Both UARTs
//!             - for AT90CAN128/64/32
//!
//! This file can be parsed by Doxygen for automatic documentation generation.
//! This file has been validated with AVRStudio-413528/WinAVR-20070122.
//!
//! @version $Revision: 319852 $ $Name: jtellier $
//!
//! @todo
//! @bug
//******************************************************************************

//_____  I N C L U D E S _______________________________________________________

#include "config.h"
#include "uart_gcc_drv.h"
#include <stdio.h>

//_____ M A C R O S ____________________________________________________________

//_____ D E F I N I T I O N S __________________________________________________

//_____ D E C L A R A T I O N S ________________________________________________


//******************************************************************************
//  @fn uart_init
//!
//! UART initialization. Initialize the UART refered by USE_UART definition
//! to BAUDRATE speed. 
//! Initialization details: 8-bit, no parity, 2 stop-bit & no interrupt.     
//!
//! @warning Set definition for BAUDRATE
//!
//! @param (none)
//!
//! @return (none)
//!
//******************************************************************************
void uart_init(void)
{
    UCSRB_N = 0 ;                     // Disable UART
    Uart_set_baudrate(UART_BAUDRATE); // Macro in "uart_no_int_lib.h"
    UCSRA_N |= (1<<UDRE_N);
    UCSRC_N  = (1<<USBS_N) | (1<<UCSZ1_N) | (1<<UCSZ0_N);
    UCSRB_N  = (1<<RXEN_N) | (1<<TXEN_N);  // then, (re)enable UART
}

//******************************************************************************
//  @fn get_char_uart
//!
//! Receive a character from the UART Rx.
//!
//! This features a simple line-editor that allows to delete and
//! re-edit the characters entered, until either CR or NL is entered.
//! Printable characters entered will be echoed using uart_put_char().
//!
//! Editing characters:
//!   - \b (BS) or \177 (DEL) delete the previous character
//!   - ^u kills the entire input buffer
//!   - ^w deletes the previous word
//!   - ^r sends a CR, and then reprints the buffer
//!   - \t will be replaced by a single space
//!
//! All other control characters will be ignored.
//!
//! The internal line buffer is RX_BUFSIZE (80) characters long, which
//! includes the terminating \n (but no terminating \0).  If the buffer
//! is full (i. e., at RX_BUFSIZE-1 characters in order to keep space for
//! the trailing \n), any further input attempts will send a \a to
//! uart_put_char() (BEL character), although line editing is still
//! allowed.
//!
//! Input errors while talking to the UART will cause an immediate
//! return of -1 (error indication).  Notably, this will be caused by a
//! framing error (e. g. serial line "break" condition), by an input
//! overrun, and by a parity error (if parity was enabled and automatic
//! parity recognition is supported by hardware).
//!
//! Successive calls to uart_get_char() will be satisfied from the
//! internal buffer until that buffer is emptied again.
//!
//!
//!
//! Get byte from RX UART. If UART has received a character, this character
//! is returned, if no character has been received, 0x00 is returned.
//!
//! @warning "uart_init()" must be performed before
//!
//! @param (none)
//!
//! @return Character from RX UART or 0x00 if nothing received
//!
//******************************************************************************
int uart_get_char(FILE *stream)
{
    U8 c;
    char *cp, *cp2;
    static char b[RX_BUFSIZE];
    static char *rxp;

    if (rxp == 0)
    {
        for (cp = b;;)
        {
            while ( !(UCSRA_N & (1<<RXC_N)) );
            
            if (UCSRA_N & (1<<FE_N))  return _FDEV_EOF;
	        if (UCSRA_N & (1<<DOR_N)) return _FDEV_ERR;
	        c = UDR_N;
	        
	        //! Behaviour similar to Unix stty ICRNL */
	        if (c == '\r') c = '\n';
	        if (c == '\n')
	        {
	            *cp = c;
	            uart_put_char(c, stream);
	            rxp = b;
	            break;
	        }
	        else if (c == '\t') c = ' ';

	        if ((c >= (U8)' ' && c <= (U8)'\x7e') || c >= (U8)'\xa0')
	        {
	            if (cp == b + RX_BUFSIZE - 1) uart_put_char('\a', stream);
	            else
	            {
		            *cp++ = c;
		            uart_put_char(c, stream);
	            }
	         continue;
	        }

	        switch (c)
	        {
	            case 'c' & 0x1f:
	                return -1;
                
	            case '\b':
	            case '\x7f':
	                if (cp > b)
	                {
		                uart_put_char('\b', stream);
		                uart_put_char(' ' , stream);
		                uart_put_char('\b', stream);
		                cp--;
	                }
	                break;
                
	            case 'r' & 0x1f:
	                uart_put_char('\r', stream);
	                for (cp2 = b; cp2 < cp; cp2++) uart_put_char(*cp2, stream);
	                break;
                
	            case 'u' & 0x1f:
	                while (cp > b)
	                {
		                uart_put_char('\b', stream);
		                uart_put_char(' ' , stream);
		                uart_put_char('\b', stream);
		                cp--;
	                }
	                break;
                
	            case 'w' & 0x1f:
	                while (cp > b && cp[-1] != ' ')
	                {
		                uart_put_char('\b', stream);
		                uart_put_char(' ' , stream);
		                uart_put_char('\b', stream);
		                cp--;
	                }
	                break;
	        }
        }
    }

    c = *rxp++;
    if (c == '\n') rxp = 0;

    return c;
}

//******************************************************************************
//  @fn uart_put_char
//!
//! Send a character down the UART Tx and wait until tx holding reg. is empty.
//!
//! @warning "uart_init()" must be performed before
//!
//! @param  character to send
//!
//! @return (0)
//!
//******************************************************************************
int uart_put_char (char c2s, FILE *stream)
{
    if (c2s == '\a')
    {
        fputs("*ring*\n", stderr);
        return 0;
    }

    if (c2s == '\n')
    {
        uart_put_char('\r', stream);
    }
    while ( !(UCSRA_N & (1<<UDRE_N)) ); // wait for empty transmit buffer 
    UDR_N = c2s;
    
    return 0;
}

