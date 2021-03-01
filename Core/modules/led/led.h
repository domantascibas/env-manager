#ifndef MODULE_LED_H
#define MODULE_LED_H

#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"

#define LED_PORT GPIOB
#define LED_RED GPIO_PIN_9
#define LED_YEL GPIO_PIN_8

void led_init(void);
void led_on(uint16_t led);
void led_off(uint16_t led);
void led_toggle(uint16_t led);

#endif
