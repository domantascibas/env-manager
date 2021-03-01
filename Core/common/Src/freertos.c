#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "led.h"

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */
void vLedAliveTask(void *pvParameters);

void MX_FREERTOS_Init(void) {
    xTaskCreate(vLedAliveTask, "LedAlive", 128, NULL, 1, NULL);
}

void vLedAliveTask(void *pvParameters) {
    portTickType xLastWakeTime;
    const portTickType xFrequency = 1000;
    xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        led_toggle(LED_RED);
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
