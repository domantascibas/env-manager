#include "hd44780.h"
#include "i2c_manager.h"

#if ARDUINO_LIB_EN
uint8_t _displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
uint8_t _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
uint8_t _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
uint8_t _numlines = 2;
uint8_t _backlightval = LCD_NOBACKLIGHT;

void write4bits(uint8_t value);
void expanderWrite(uint8_t _data);
void pulseEnable(uint8_t _data);

inline void command(uint8_t value);
inline size_t write(uint8_t value);
void send(uint8_t value, uint8_t mode);


/********** high level commands, for the user! */
void clear() {
    command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
    vTaskDelay(2);  // this command takes a long time!
}

void home() {
    command(LCD_RETURNHOME);  // set cursor position to zero
    vTaskDelay(2);  // this command takes a long time!
}

void setCursor(uint8_t col, uint8_t row) {
    int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
    if (row > _numlines) {
        row = _numlines - 1;  // we count rows starting w/0
    }
    command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void noDisplay() {
    _displaycontrol &= ~LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void display() {
    _displaycontrol |= LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void noCursor() {
    _displaycontrol &= ~LCD_CURSORON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void cursor() {
    _displaycontrol |= LCD_CURSORON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void noBlink() {
    _displaycontrol &= ~LCD_BLINKON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void blink() {
    _displaycontrol |= LCD_BLINKON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn the (optional) backlight off/on
void noBacklight(void) {
    _backlightval = LCD_NOBACKLIGHT;
    expanderWrite(0);
}

void backlight(void) {
    _backlightval = LCD_BACKLIGHT;
    expanderWrite(0);
}

/*********** mid level commands, for sending data/cmds */

inline void command(uint8_t value) {
    send(value, 0);
}

inline size_t write(uint8_t value) {
    send(value, 1);
    return 0;
}

/************ low level data pushing commands **********/

// write either command or data
void send(uint8_t value, uint8_t mode) {
    uint8_t highnib = value & 0xf0;
    uint8_t lownib = (value << 4) & 0xf0;
    write4bits((highnib) | mode);
    write4bits((lownib) | mode);
}

void write4bits(uint8_t value) {
    expanderWrite(value);
    pulseEnable(value);
}

void expanderWrite(uint8_t _data) {
    uint8_t data = _data | _backlightval;
    i2c_write(I2C_ADDR_LCD_HD44780, &data, 1);
}

void pulseEnable(uint8_t _data) {
    expanderWrite(_data | En);  // En high
    vTaskDelay(1);           // enable pulse must be >450ns

    expanderWrite(_data & ~En); // En low
    // vTaskDelay(1);          // commands need > 37us to settle
}


void lcd_send_cmd(char cmd) {
    char data_u, data_l;
    uint8_t data_t[4];
    data_u = (cmd & 0xf0);
    data_l = ((cmd << 4) & 0xf0);
    data_t[0] = data_u | 0x0C; //en=1, rs=0
    data_t[1] = data_u | 0x08; //en=0, rs=0
    data_t[2] = data_l | 0x0C; //en=1, rs=0
    data_t[3] = data_l | 0x08; //en=0, rs=0
    // HAL_I2C_Master_Transmit(&hi2c1, I2C_ADDR_LCD_HD44780, (uint8_t *) data_t, 4, 100);
    i2c_write(I2C_ADDR_LCD_HD44780, data_t, 4);
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
    // HAL_I2C_Master_Transmit(&hi2c1, I2C_ADDR_LCD_HD44780, (uint8_t *) data_t, 4, 100);
    i2c_write(I2C_ADDR_LCD_HD44780, data_t, 4);
}

void lcd_clear(void) {
    lcd_send_cmd(0x80);
    for (int i = 0; i < 70; i++) {
        lcd_send_data(' ');
    }
}

void lcd_put_cur(int row, int col) {
    switch (row) {
        case 0:
            col |= 0x80;
            break;
        case 1:
            col |= 0xC0;
            break;
    }

    lcd_send_cmd(col);
}

void lcd_init(void) {
    // 4 bit initialisation
    vTaskDelay(50);  // wait for >40ms
    write4bits(0x03 << 4);
    vTaskDelay(5);  // wait for >4.1ms
    write4bits(0x03 << 4);
    vTaskDelay(5);  // wait for >100us
    write4bits(0x03 << 4);
    vTaskDelay(1);
    write4bits(0x02 << 4);   // 4bit mode
    vTaskDelay(10);

    // set # lines, font size, etc.
    command(LCD_FUNCTIONSET | _displayfunction);

    // turn the display on with no cursor or blinking default
    _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    display();

    // clear it off
    clear();

    // Initialize to default text direction (for roman languages)
    _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

    // set the entry mode
    command(LCD_ENTRYMODESET | _displaymode);

    home();

    backlight();
}

void lcd_send_string(char *str) {
    while (*str) lcd_send_data(*str++);
}

#else

/*
 * Adapted from:
 * Olivier Van den Eede
 * https://github.com/4ilo/HD44780-Stm32HAL
 */

const uint8_t ROW_16[] = {0x00, 0x40, 0x10, 0x50};
const uint8_t ROW_20[] = {0x00, 0x40, 0x14, 0x54};
/************************************** Static declarations **************************************/

static void lcd_write_data(LiquidCrystal_I2C *lcd, uint8_t data);
static void lcd_write_command(LiquidCrystal_I2C *lcd, uint8_t command);
// static void lcd_write(LiquidCrystal_I2C *lcd, uint8_t data, uint8_t len);

static void send(uint8_t value, uint8_t mode);
static void write4bits(uint8_t value);
static void expanderWrite(uint8_t _data);
static void pulseEnable(uint8_t _data);

/************************************** Function definitions **************************************/

/**
 * Create new LiquidCrystal_I2C and initialize the Lcd
 */
LiquidCrystal_I2C Lcd_create(uint8_t addr, uint8_t cols, uint8_t rows, uint8_t mode) {
    LiquidCrystal_I2C lcd;

    lcd.cols = cols;
    lcd.rows = rows;
    lcd.mode = mode;

    Lcd_init(&lcd);
    return lcd;
}

/**
 * Initialize 16x2-lcd without cursor
 */
void Lcd_init(LiquidCrystal_I2C *lcd) {
    if (lcd->mode == LCD_4_BIT_MODE) {
        // 4 bit initialisation
        vTaskDelay(50);  // wait for >40ms
        write4bits(0x03 << 4);
        vTaskDelay(5);  // wait for >4.1ms
        write4bits(0x03 << 4);
        vTaskDelay(5);  // wait for >100us
        write4bits(0x03 << 4);
        vTaskDelay(1);
        write4bits(0x02 << 4);   // 4bit mode
        vTaskDelay(10);

        // dislay initialisation
        lcd_write_command(lcd, 0x2C);  // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
        vTaskDelay(1);
        lcd_write_command(lcd, 0x0E);  //Display on/off control --> D=0,C=0, B=0  ---> display off
        vTaskDelay(1);
        // lcd_write_command(lcd, 0x01);   // clear display
        vTaskDelay(1);
        lcd_write_command(lcd, 0x06);  //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
        vTaskDelay(1);
        // lcd_write_command(lcd, 0x0C);  //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
        // vTaskDelay(1);

        lcd_write_command(lcd, FUNCTION_SET | OPT_N);               // 4-bit mode
    } else {
        lcd_write_command(lcd, FUNCTION_SET | OPT_DL | OPT_N);
    }

    lcd_write_command(lcd, CLEAR_DISPLAY);                      // Clear screen
    lcd_write_command(lcd, DISPLAY_ON_OFF_CONTROL | OPT_D);     // Lcd-on, cursor-off, no-blink
    lcd_write_command(lcd, ENTRY_MODE_SET | OPT_INC);           // Increment cursor
}

/**
 * Write a number on the current position
 */
void Lcd_int(LiquidCrystal_I2C *lcd, int number) {
    char buffer[11];
    sprintf(buffer, "%d", number);

    Lcd_string(lcd, buffer);
}

/**
 * Write a string on the current position
 */
void Lcd_string(LiquidCrystal_I2C *lcd, char *string) {
    for (uint8_t i = 0; i < strlen(string); i++) {
        lcd_write_data(lcd, string[i]);
    }
}

/**
 * Set the cursor position
 */
void Lcd_cursor(LiquidCrystal_I2C *lcd, uint8_t row, uint8_t col) {
#ifdef LCD20xN
    lcd_write_command(lcd, SET_DDRAM_ADDR + ROW_20[row] + col);
#endif

#ifdef LCD16xN
    lcd_write_command(lcd, SET_DDRAM_ADDR + ROW_16[row] + col);
#endif
}

/**
 * Clear the screen
 */
void Lcd_clear(LiquidCrystal_I2C *lcd) {
    lcd_write_command(lcd, CLEAR_DISPLAY);
}

void Lcd_define_char(LiquidCrystal_I2C *lcd, uint8_t code, uint8_t bitmap[]) {
    lcd_write_command(lcd, SETCGRAM_ADDR + (code << 3));
    for (uint8_t i = 0; i < 8; ++i) {
        lcd_write_data(lcd, bitmap[i]);
    }

}


/************************************** Static function definition **************************************/

/**
 * Write a byte to the command register
 */
void lcd_write_command(LiquidCrystal_I2C *lcd, uint8_t command) {
    send(command, 0);
    // // i2c_write(I2C_ADDR_LCD_HD44780, );
    // // HAL_GPIO_WritePin(lcd->rs_port, lcd->rs_pin, LCD_COMMAND_REG);      // Write to command register

    // if (lcd->mode == LCD_4_BIT_MODE) {
    //     lcd_write(lcd, (command >> 4), LCD_NIB);
    //     lcd_write(lcd, command & 0x0F, LCD_NIB);
    // } else {
    //     lcd_write(lcd, command, LCD_BYTE);
    // }
}

/**
 * Write a byte to the data register
 */
void lcd_write_data(LiquidCrystal_I2C *lcd, uint8_t data) {
    send(data, 1);
    // // HAL_GPIO_WritePin(lcd->rs_port, lcd->rs_pin, LCD_DATA_REG);         // Write to data register

    // if (lcd->mode == LCD_4_BIT_MODE) {
    //     lcd_write(lcd, data >> 4, LCD_NIB);
    //     lcd_write(lcd, data & 0x0F, LCD_NIB);
    // } else {
    //     lcd_write(lcd, data, LCD_BYTE);
    // }

}

/**
 * Set len bits on the bus and toggle the enable line
 */
// void lcd_write(LiquidCrystal_I2C *lcd, uint8_t data, uint8_t len) {
//     for (uint8_t i = 0; i < len; i++) {
//         // HAL_GPIO_WritePin(lcd->data_port[i], lcd->data_pin[i], (data >> i) & 0x01);
//     }

//     // HAL_GPIO_WritePin(lcd->en_port, lcd->en_pin, 1);
//     DELAY(1);
//     // HAL_GPIO_WritePin(lcd->en_port, lcd->en_pin, 0);        // Data receive on falling edge
// }

// write either command or data
void send(uint8_t value, uint8_t mode) {
    uint8_t highnib = value & 0xf0;
    uint8_t lownib = (value << 4) & 0xf0;
    write4bits((highnib) | mode);
    write4bits((lownib) | mode);
}

void write4bits(uint8_t value) {
    expanderWrite(value);
    pulseEnable(value);
}

void expanderWrite(uint8_t _data) {
    uint8_t data = _data | LCD_BACKLIGHT;
    i2c_write(I2C_ADDR_LCD_HD44780, &data, 1);
}

void pulseEnable(uint8_t _data) {
    expanderWrite(_data | En);  // En high
    vTaskDelay(1);           // enable pulse must be >450ns

    expanderWrite(_data & ~En); // En low
    // vTaskDelay(1);          // commands need > 37us to settle
}

#endif
