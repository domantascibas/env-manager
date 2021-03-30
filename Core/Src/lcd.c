#include "lcd.h"
#include "hd44780.h"
// #include "stm32f1xx_hal.h"
#include "uart.h"

void _tLcdDisplay(void *arguments) {
    while (1) {
        PTS("LCD");
        vTaskDelay(2000);
    }
}
