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
    led_init();

    for (;;) {
        led_toggle(LED_GREEN);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
