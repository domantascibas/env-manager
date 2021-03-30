#ifndef _I2C_MANAGER_H_
#define _I2C_MANAGER_H_

#include "FreeRTOS.h"
#include "task.h"

#define I2C_ADDR_LIGHT_BH1750       0x23
#define I2C_ADDR_LCD_HD44780        0x27
#define I2C_ADDR_TEMP_BMP180        0x77

void _tI2C(void *arguments);

#endif
