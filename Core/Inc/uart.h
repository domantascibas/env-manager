#ifndef _UART_H
#define _UART_H

#include "stm32f1xx_hal.h"
#include "stdarg.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

#define MAX_MESSAGE_LENGTH          64

void uart_init(void);
void PTS(char *string);
void PTS_d(const char *module, const char *format, ...);
// void PTS_t(int num, char *string);
void PTS_f(const char *format, ...);
void PTS_df(const char *module, const char *format, ...);
// void PTS_tf(int num, const char *format, ...);

#endif
