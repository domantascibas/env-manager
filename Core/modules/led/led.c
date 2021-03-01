#include "led.h"

void led_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = LED_RED | LED_YEL;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(LED_PORT, &GPIO_InitStructure);
    HAL_GPIO_WritePin(LED_PORT, LED_RED | LED_YEL, GPIO_PIN_RESET);
}

void led_on(uint16_t led) {
    HAL_GPIO_WritePin(LED_PORT, led, GPIO_PIN_SET);
}

void led_off(uint16_t led) {
    HAL_GPIO_WritePin(LED_PORT, led, GPIO_PIN_RESET);
}

void led_toggle(uint16_t led) {
    HAL_GPIO_TogglePin(LED_PORT, led);
}
