#include "status_led.h"
#include "stm32f1xx_hal.h"
#include "uart.h"

void _tStatusLed(void *arguments) {
    while (1) {
        vTaskDelay(4500);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
        vTaskDelay(100);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
    }
}

void _tPrintBlink(void *arguments) {
    while (1) {
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_8);
        PTS_f("blink: %d", xTaskGetTickCount());
        vTaskDelay(1006);
    }
}
