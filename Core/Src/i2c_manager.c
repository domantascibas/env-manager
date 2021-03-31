#include "i2c_manager.h"
#include "task_manager.h"
#include "i2c.h"
#include "uart.h"

#define I2C_MIN_ADDRESS     0x03
#define I2C_MAX_ADDRESS     0x78
#define I2C_TASK_DELAY      5000
#define I2C_MAX_DEVICES     4

static const char moduleStr[] = "I2C";
#define PTS_dbg(fmt, ...) PTS_d(moduleStr, fmt, ##__VA_ARGS__)
#define PTS_dbg_f(fmt, ...) PTS_df(moduleStr, fmt, ##__VA_ARGS__)

void _tI2C(void *arguments) {
    MX_I2C1_Init();

    uint8_t devices[I2C_MAX_DEVICES];
    static uint8_t present = 0;
    uint8_t found, address = 0;

    while (1) {
        found = 0;
        address = 0;
        memset(devices, 0, I2C_MAX_DEVICES);

        for (uint8_t i = I2C_MIN_ADDRESS; i < I2C_MAX_ADDRESS; i++) {
            address = i << 1;
            if (HAL_OK == HAL_I2C_IsDeviceReady(&hi2c1, address, 3, 10)) {
                devices[found] = address >> 1;
                found++;
            }
        }

        if (found == 0) {
            PTS_dbg("I2C rst");
            I2C_reset();
        } else {
            if (present != found) {
                PTS_dbg("Devices update");
                present = found;
                for (uint8_t i = 0; i < present; i++) {
                    char *msg;
                    switch (devices[i]) {
                        case I2C_ADDR_LCD_HD44780:
                            msg = "HD44780 lcd";
                            break;

                        case I2C_ADDR_LIGHT_BH1750:
                            msg = "BH1750 light";
                            break;

                        case I2C_ADDR_TEMP_BMP180:
                            msg = "BMP180 temp";
                            break;

                        default:
                            msg = "unknown device";
                            break;
                    }
                    PTS_dbg_f("found[%d] :0x%02x %s", present, devices[i], msg);
                }
            }
            vTaskDelay(I2C_TASK_DELAY);
        }
    }
}

void i2c_write(uint8_t addr, uint8_t *data, uint8_t size) {
    HAL_I2C_Master_Transmit(&hi2c1, addr << 1, data, size, 100);
}
