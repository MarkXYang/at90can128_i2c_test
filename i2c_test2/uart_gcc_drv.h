//******************************************************************************
//! @file $RCSfile: uart_gcc_drv.h,v $
//!
//! Copyright (c) 2007 Atmel.
//!
//! Use of this program is subject to Atmel's End User License Agreement.
//! Please read file license.txt for copyright notice.
//!
//! @brief UART include file working with GCC library.
//!        This file contains the prototypes and the macros of the
//!        low level functions (drivers) of:
//!             - Both UARTs
//!             - for AT90CAN128/64/32.
//!
//! This file can be parsed by Doxygen for automatic documentation generation.
//! This file has been validated with AVRStudio-413528/WinAVR-20070122.
//!
//! @version $Revision: 319852 $ $Name: jtellier $
//!
//! @todo
//! @bug
//******************************************************************************

#ifndef _UART_GCC_DRV_H_
#define _UART_GCC_DRV_H_

//_____ I N C L U D E S ________________________________________________________

#include "config.h"

//_____ D E F I N I T I O N S __________________________________________________

#ifndef FOSC
        #error You must define FOSC in config.h
#endif

#ifndef USE_UART
        #error You must define USE_UART to 0 or 1
    #elif USE_UART == 0
        #define UBRRH_N   (UBRR0H)
        #define UBRRL_N   (UBRR0L)                   
        #define UCSRA_N   (UCSR0A)
          #define U2X_N   (U2X0)
          #define UDRE_N  (UDRE0)
          #define RXC_N   (RXC0)
          #define FE_N    (FE0)
          #define DOR_N   (DOR0)
        #define UCSRB_N   (UCSR0B)
          #define USBS_N  (USBS0)
          #define UCSZ1_N (UCSZ01)
          #define UCSZ0_N (UCSZ00)
        #define UCSRC_N   (UCSR0C)
          #define RXEN_N  (RXEN0)
          #define TXEN_N  (TXEN0)
        #define UDR_N     (UDR0)
    #elif USE_UART == 1
        #define UBRRH_N   (UBRR1H)
        #define UBRRL_N   (UBRR1L)                   
        #define UCSRA_N   (UCSR1A)
          #define U2X_N   (U2X1)
          #define UDRE_N  (UDRE1)
          #define RXC_N   (RXC1)
          #define FE_N    (FE1)
          #define DOR_N   (DOR1)
        #define UCSRB_N   (UCSR1B)
          #define USBS_N  (USBS1)
          #define UCSZ1_N (UCSZ11)
          #define UCSZ0_N (UCSZ10)
        #define UCSRC_N   (UCSR1C)
          #define RXEN_N  (RXEN1)
          #define TXEN_N  (TXEN1)
        #define UDR_N     (UDR1)
    #else
        #error Highest USE_UART definition
#endif

//Size of internal line buffer used by uart_get_char()
#define RX_BUFSIZE 80

//_____ M A C R O S ____________________________________________________________

//******************************************************************************
//! Set UART baudrate.
//!
//! @param bdr   (c.f. Examples of baud rate setting in device data sheet)
//******************************************************************************
#define Uart_set_baudrate(bdr) { UBRRH_N = (U8)((((((((U32)FOSC*1000)<<1)/((U32)bdr*8))+1)>>1)-1)>>8); \
                                 UBRRL_N = (U8)(( (((((U32)FOSC*1000)<<1)/((U32)bdr*8))+1)>>1)-1)    ; \
                                 UCSRA_N |=  (1<<U2X_N); }

//_____ D E C L A R A T I O N S ________________________________________________


//_____ P R O T O T Y P E S - D E C L A R A T I O N ____________________________

//******************************************************************************
//! Perform UART startup initialization.
//
void	uart_init(void);

//******************************************************************************
//! Receive one character from the UART.  The actual reception is
//! line-buffered, and one character is returned from the buffer at
//! each invokation.
//
int	uart_get_char(FILE *stream);

//******************************************************************************
//! Send to the UART.
//
int	uart_put_char(char c2s, FILE *stream);

//______________________________________________________________________________

#endif  /* _UART_GCC_DRV_H_ */
