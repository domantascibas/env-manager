#ifndef DRIVER_RESET_SOURCE_
#define DRIVER_RESET_SOURCE_

#include "stm32f1xx_hal.h"

#define BOR_RS      0x02    // Set by hardware when a POR/PDR or BOR reset occurs
#define PIN_RS      0x04    // reset pin
#define POR_RS      0x08    // power on/down reset
#define SFT_RS      0x10    // software reset
#define IWDG_RS     0x20    // independent watchdog
#define WWDG_RS     0x40    // window watchdog
#define LPWR_RS     0x80    // Low power reset

uint8_t get_mcu_reset_source(void);

#endif
