#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "iwdg.h"
#include "uart.h"

static const char moduleStr[] = "FRTOS_TSK";
#define PTS_dbg(fmt, ...) PTS_d(moduleStr, fmt, ##__VA_ARGS__)
#define PTS_dbg_f(fmt, ...) PTS_df(moduleStr, fmt, ##__VA_ARGS__)

xTaskHandle iwdgResetHandle;
xTaskHandle statusLedHandle;
xTaskHandle printBlinkHandle;

void iwdgResetTask(void *argument);
void statusLedTask(void *argument);
void printBlinkTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

void MX_FREERTOS_Init(void) {
    // xTaskCreate(statusLedTask, "statusLed", configMINIMAL_STACK_SIZE, NULL, 16, &statusLedHandle);
    xTaskCreate(printBlinkTask, "blink", configMINIMAL_STACK_SIZE, NULL, 16, &printBlinkHandle);
}

void iwdgResetTask(void *argument) {
    while (1) {
        PTS_dbg("wdg upd");
        vTaskDelay(4500);
        iwdgReset();
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
        vTaskDelay(100);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
    }
}

void statusLedTask(void *argument) {
    while (1) {
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9);
        vTaskDelay(100);
    }
}

void printBlinkTask(void *argument) {
    while (1) {
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_8);
        PTS_dbg("blink: %d", xTaskGetTickCount());
        vTaskDelay(1006);
    }
}
