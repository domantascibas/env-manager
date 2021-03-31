#include "lcd.h"
#include "hd44780.h"
#include "uart.h"
#include "i2c_manager.h"

void _tLcdDisplay(void *arguments) {

#if ARDUINO_LIB_EN
    uint8_t row = 0;
    uint8_t col = 0;

    lcd_init();
    lcd_send_string("HELLO WORLD");
    vTaskDelay(1000);
    lcd_put_cur(1, 0);
    lcd_send_string("from CTECH");
    vTaskDelay(2000);
    lcd_clear();
#else
    LiquidCrystal_I2C lcd = Lcd_create(I2C_ADDR_LCD_HD44780, 16, 2, LCD_4_BIT_MODE);
    Lcd_string(&lcd, "4ilo");
    Lcd_cursor(&lcd, 1, 6);
    Lcd_int(&lcd, -500);
#endif

    while (1) {
#if ARDUINO_LIB_EN
        for (int i = 0; i < 128; i++) {
            lcd_put_cur(row, col);

            lcd_send_data(i + 48);

            col++;

            if (col > 15) {
                row++;
                col = 0;
            }
            if (row > 1) {
                row = 0;
            }

            vTaskDelay(250);
        }
#endif
    }
}
