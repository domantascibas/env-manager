#include "lcd.h"
#include "stm32f1xx_hal.h"
#include "uart.h"
#include "i2c_manager.h"

// check if address 0x27 is available, then start lcd task, else if address is not available, stop it, periodically check if lcd is connected

void _tLcdDisplay(void *arguments) {
    for (;;) {

    }
}
