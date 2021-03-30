#include "hd44780.h"
#include "i2c_manager.h"

extern I2C_HandleTypeDef hi2c1;  // change your handler here accordingly

void lcd_send_cmd(char cmd) {
    char data_u, data_l;
    uint8_t data_t[4];
    data_u = (cmd & 0xf0);
    data_l = ((cmd << 4) & 0xf0);
    data_t[0] = data_u | 0x0C; //en=1, rs=0
    data_t[1] = data_u | 0x08; //en=0, rs=0
    data_t[2] = data_l | 0x0C; //en=1, rs=0
    data_t[3] = data_l | 0x08; //en=0, rs=0

    i2c_write(I2C_ADDR_LCD_HD44780, (uint8_t *) data_t, 4);
}

void lcd_send_data(char data) {
    char data_u, data_l;
    uint8_t data_t[4];
    data_u = (data & 0xf0);
    data_l = ((data << 4) & 0xf0);
    data_t[0] = data_u | 0x0D; //en=1, rs=1
    data_t[1] = data_u | 0x09; //en=0, rs=1
    data_t[2] = data_l | 0x0D; //en=1, rs=1
    data_t[3] = data_l | 0x09; //en=0, rs=1

    i2c_write(I2C_ADDR_LCD_HD44780, (uint8_t *) data_t, 4);
}

void lcd_clear(void) {
    lcd_send_cmd(0x00);
    for (int i = 0; i < 100; i++) {
        lcd_send_data(' ');
    }
}

void lcd_init(void) {
    // 4 bit initialisation
    HAL_Delay(50);  // wait for >40ms
    lcd_send_cmd(0x30);
    HAL_Delay(5);  // wait for >4.1ms
    lcd_send_cmd(0x30);
    HAL_Delay(1);  // wait for >100us
    lcd_send_cmd(0x30);
    HAL_Delay(10);
    lcd_send_cmd(0x20);   // 4bit mode
    HAL_Delay(10);

    // dislay initialisation
    lcd_send_cmd(0x28);  // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
    HAL_Delay(1);
    lcd_send_cmd(0x08);  //Display on/off control --> D=0,C=0, B=0  ---> display off
    HAL_Delay(1);
    lcd_send_cmd(0x01);   // clear display
    HAL_Delay(1);
    HAL_Delay(1);
    lcd_send_cmd(0x06);  //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
    HAL_Delay(1);
    lcd_send_cmd(0x0C);  //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
}

void lcd_send_string(char *str) {
    while (*str) lcd_send_data(*str++);
}


// /*
//  * Adapted from:
//  * Olivier Van den Eede
//  * https://github.com/4ilo/HD44780-Stm32HAL
//  */

// #include "hd44780.h"
// #include "i2c_manager.h"

// const uint8_t ROW_16[] = {0x00, 0x40, 0x10, 0x50};
// const uint8_t ROW_20[] = {0x00, 0x40, 0x14, 0x54};
// /************************************** Static declarations **************************************/

// static void lcd_write_data(LiquidCrystal_I2C *lcd, uint8_t data);
// static void lcd_write_command(LiquidCrystal_I2C *lcd, uint8_t command);
// static void lcd_write(LiquidCrystal_I2C *lcd, uint8_t data, uint8_t len);


// /************************************** Function definitions **************************************/

// /**
//  * Create new LiquidCrystal_I2C and initialize the Lcd
//  */
// LiquidCrystal_I2C Lcd_create(uint8_t addr, uint8_t cols, uint8_t rows) {
//     LiquidCrystal_I2C lcd;

//     lcd.cols = cols;
//     lcd.rows = rows;
//     lcd.mode = LCD_4_BIT_MODE;

//     Lcd_init(&lcd);

//     return lcd;
// }

// /**
//  * Initialize 16x2-lcd without cursor
//  */
// void Lcd_init(LiquidCrystal_I2C *lcd) {
//     if (lcd->mode == LCD_4_BIT_MODE) {
//         lcd_write_command(lcd, 0x33);
//         lcd_write_command(lcd, 0x32);
//         lcd_write_command(lcd, FUNCTION_SET | OPT_N);               // 4-bit mode
//     } else
//         lcd_write_command(lcd, FUNCTION_SET | OPT_DL | OPT_N);


//     lcd_write_command(lcd, CLEAR_DISPLAY);                      // Clear screen
//     lcd_write_command(lcd, DISPLAY_ON_OFF_CONTROL | OPT_D);     // Lcd-on, cursor-off, no-blink
//     lcd_write_command(lcd, ENTRY_MODE_SET | OPT_INC);           // Increment cursor
// }

// /**
//  * Write a number on the current position
//  */
// void Lcd_int(LiquidCrystal_I2C *lcd, int number) {
//     char buffer[11];
//     sprintf(buffer, "%d", number);

//     Lcd_string(lcd, buffer);
// }

// /**
//  * Write a string on the current position
//  */
// void Lcd_string(LiquidCrystal_I2C *lcd, char *string) {
//     for (uint8_t i = 0; i < strlen(string); i++) {
//         lcd_write_data(lcd, string[i]);
//     }
// }

// /**
//  * Set the cursor position
//  */
// void Lcd_cursor(LiquidCrystal_I2C *lcd, uint8_t row, uint8_t col) {
// #ifdef LCD20xN
//     lcd_write_command(lcd, SET_DDRAM_ADDR + ROW_20[row] + col);
// #endif

// #ifdef LCD16xN
//     lcd_write_command(lcd, SET_DDRAM_ADDR + ROW_16[row] + col);
// #endif
// }

// /**
//  * Clear the screen
//  */
// void Lcd_clear(LiquidCrystal_I2C *lcd) {
//     lcd_write_command(lcd, CLEAR_DISPLAY);
// }

// void Lcd_define_char(LiquidCrystal_I2C *lcd, uint8_t code, uint8_t bitmap[]) {
//     lcd_write_command(lcd, SETCGRAM_ADDR + (code << 3));
//     for (uint8_t i = 0; i < 8; ++i) {
//         lcd_write_data(lcd, bitmap[i]);
//     }

// }


// /************************************** Static function definition **************************************/

// /**
//  * Write a byte to the command register
//  */
// void lcd_write_command(LiquidCrystal_I2C *lcd, uint8_t command) {
//     // i2c_write(I2C_ADDR_LCD_HD44780, );
//     // HAL_GPIO_WritePin(lcd->rs_port, lcd->rs_pin, LCD_COMMAND_REG);      // Write to command register

//     if (lcd->mode == LCD_4_BIT_MODE) {
//         lcd_write(lcd, (command >> 4), LCD_NIB);
//         lcd_write(lcd, command & 0x0F, LCD_NIB);
//     } else {
//         lcd_write(lcd, command, LCD_BYTE);
//     }

// }

// /**
//  * Write a byte to the data register
//  */
// void lcd_write_data(LiquidCrystal_I2C *lcd, uint8_t data) {
//     // HAL_GPIO_WritePin(lcd->rs_port, lcd->rs_pin, LCD_DATA_REG);         // Write to data register

//     if (lcd->mode == LCD_4_BIT_MODE) {
//         lcd_write(lcd, data >> 4, LCD_NIB);
//         lcd_write(lcd, data & 0x0F, LCD_NIB);
//     } else {
//         lcd_write(lcd, data, LCD_BYTE);
//     }

// }

// /**
//  * Set len bits on the bus and toggle the enable line
//  */
// void lcd_write(LiquidCrystal_I2C *lcd, uint8_t data, uint8_t len) {
//     for (uint8_t i = 0; i < len; i++) {
//         // HAL_GPIO_WritePin(lcd->data_port[i], lcd->data_pin[i], (data >> i) & 0x01);
//     }

//     // HAL_GPIO_WritePin(lcd->en_port, lcd->en_pin, 1);
//     DELAY(1);
//     // HAL_GPIO_WritePin(lcd->en_port, lcd->en_pin, 0);        // Data receive on falling edge
// }
