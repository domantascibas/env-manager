#ifndef _UART_H
#define _UART_H

#include "stm32f1xx_hal.h"

#define uDEBUG              USART3
#define MSG_SIZE            512

extern char MSG[MSG_SIZE];

void uart_init(void);
void uart_print(char *msg, uint16_t len);

#endif
