//
//***** config.h ***************************************************************

#include <stdio.h>
#include <avr/io.h>
#include "compiler.h"

#define __STDIO_FDEVOPEN_COMPAT_12

// MCU Configuration
#define FOSC           8000        // 8.000 MHz external crystal
#define F_CPU          (FOSC*1000) // Need for AVR GCC

// UART Configuration
#define USE_UART       0
#define UART_BAUDRATE  38400       // in bauds



