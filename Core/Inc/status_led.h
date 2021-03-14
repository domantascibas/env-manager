#ifndef _STATUS_LED_H_
#define _STATUS_LED_H_

#include "FreeRTOS.h"
#include "task.h"

void _tStatusLed(void *argument);
void _tPrintBlink(void *argument);

#endif
