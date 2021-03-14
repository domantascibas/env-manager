#include "status_led.h"
#include "stm32f1xx_hal.h"

xTaskHandle statusLedHandle;

void statusLedTask(void *argument) {
    while (1) {
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9);
        vTaskDelay(100);
    }
}
