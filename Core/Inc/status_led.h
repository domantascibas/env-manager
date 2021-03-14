#ifndef _STATUS_LED_H_
#define _STATUS_LED_H_

#include "FreeRTOS.h"
#include "task.h"

extern xTaskHandle statusLedHandle;
void statusLedTask(void *argument);

#endif
