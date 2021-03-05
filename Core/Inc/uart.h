#ifndef _UART_H
#define _UART_H

#include "stm32f1xx_hal.h"

void uart_init(void);
void PTS(char *string);
void PTS_f(const char *format, ...);

#endif
