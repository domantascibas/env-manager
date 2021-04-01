#ifndef _HD44780_H_
#define _HD44780_H_

#include "FreeRTOS.h"
#include "task.h"
#include "stm32f1xx_hal.h"

#define ARDUINO_LIB_EN 1

#if ARDUINO_LIB_EN
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0x04  // Enable bit
#define Rw 0x02  // Read/Write bit
#define Rs 0x01  // Register select bit

void lcd_init(void);    // initialize lcd
void lcd_send_cmd(char cmd);   // send command to the lcd
void lcd_send_data(char data);   // send data to the lcd
void lcd_send_string(char *str);   // send string to the lcd
void lcd_put_cur(int row, int col);  // put cursor at the entered position row (0 or 1), col (0-15);
void lcd_clear(void);

#else

#include "string.h"
#include "stdio.h"

// #define LCD20xN      // For 20xN LCDs
#define LCD16xN         // For 16xN LCDs

// For row start addresses
extern const uint8_t ROW_16[];
extern const uint8_t ROW_20[];

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0x04  // Enable bit
#define Rw 0x02  // Read/Write bit
#define Rs 0x01  // Register select bit

/************************************** Command register **************************************/
#define CLEAR_DISPLAY 0x01

#define RETURN_HOME 0x02

#define ENTRY_MODE_SET 0x04
#define OPT_S   0x01                    // Shift entire display to right
#define OPT_INC 0x02                    // Cursor increment

#define DISPLAY_ON_OFF_CONTROL 0x08
#define OPT_D   0x04                    // Turn on display
#define OPT_C   0x02                    // Turn on cursor
#define OPT_B   0x01                    // Turn on cursor blink

#define CURSOR_DISPLAY_SHIFT 0x10       // Move and shift cursor
#define OPT_SC 0x08
#define OPT_RL 0x04

#define FUNCTION_SET 0x20
#define OPT_DL 0x10                     // Set interface data length
#define OPT_N 0x08                      // Set number of display lines
#define OPT_F 0x04                      // Set alternate font
#define SETCGRAM_ADDR 0x040
#define SET_DDRAM_ADDR 0x80             // Set DDRAM address


/************************************** Helper macros **************************************/
#define DELAY(X) HAL_Delay(X)


/************************************** LCD defines **************************************/
#define LCD_NIB 4
#define LCD_BYTE 8
#define LCD_DATA_REG 1
#define LCD_COMMAND_REG 0


/************************************** LCD typedefs **************************************/
#define Lcd_PortType GPIO_TypeDef*
#define Lcd_PinType uint16_t

typedef enum {
    LCD_4_BIT_MODE,
    LCD_8_BIT_MODE
} Lcd_ModeTypeDef;


typedef struct {
    uint8_t addr;
    uint8_t cols;
    uint8_t rows;
    Lcd_ModeTypeDef mode;
} LiquidCrystal_I2C;


/************************************** Public functions **************************************/
LiquidCrystal_I2C Lcd_create(uint8_t addr, uint8_t cols, uint8_t rows, uint8_t mode);
void Lcd_init(LiquidCrystal_I2C *lcd);
void Lcd_int(LiquidCrystal_I2C *lcd, int number);
void Lcd_string(LiquidCrystal_I2C *lcd, char *string);
void Lcd_cursor(LiquidCrystal_I2C *lcd, uint8_t row, uint8_t col);
void Lcd_define_char(LiquidCrystal_I2C *lcd, uint8_t code, uint8_t bitmap[]);
void Lcd_clear(LiquidCrystal_I2C *lcd);

#endif /* LCD_H_ */

#endif
