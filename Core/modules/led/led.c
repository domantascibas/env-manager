#include "led.h"

void led_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;

    // GPIO_InitStructure.Pin = LED_RED | LED_YEL;
    // HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
    // HAL_GPIO_WritePin(GPIOB, LED_RED | LED_YEL, GPIO_PIN_RESET);

    GPIO_InitStructure.Pin = LED_GREEN;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
    HAL_GPIO_WritePin(GPIOC, LED_GREEN, GPIO_PIN_RESET);
}

void led_on(uint16_t led) {
    if (led == LED_GREEN) {
        HAL_GPIO_WritePin(GPIOC, led, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(GPIOB, led, GPIO_PIN_SET);
    }
}

void led_off(uint16_t led) {
    if (led == LED_GREEN) {
        HAL_GPIO_WritePin(GPIOC, led, GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(GPIOB, led, GPIO_PIN_RESET);
    }
}

void led_toggle(uint16_t led) {
    if (led == LED_GREEN) {
        HAL_GPIO_TogglePin(GPIOC, led);
    } else {
        HAL_GPIO_TogglePin(GPIOB, led);
    }
}
