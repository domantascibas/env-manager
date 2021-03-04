#include "drv_reset_source.h"
#include "uart.h"

#include "stdio.h"

uint8_t get_mcu_reset_source(void) {
    uint8_t rs = 0;
    // read pin reset flag
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST)) {
        rs |= PIN_RS;
    }
    // read POR flag
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST)) {
        rs |= POR_RS;
    }
    // read RCC_FLAG_SFTRST flag
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)) {
        rs |= SFT_RS;
    }
    // read RCC_FLAG_IWDGRST flag
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST)) {
        rs |= IWDG_RS;
    }
    // read RCC_FLAG_WWDGRST flag
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST)) {
        rs |= WWDG_RS;
    }
    // read RCC_FLAG_LPWRRST flag
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST)) {
        rs |= LPWR_RS;
    }
    __HAL_RCC_CLEAR_RESET_FLAGS();

    return rs;
}

void get_mcu_id_code(void) {
    uint32_t val = DBGMCU->IDCODE;
    uint8_t revid = '?';
    switch (val >> 16) {
        case 0x1000: revid = 'A'; break; // Revision A
        case 0x1001: revid = 'Z'; break; // Revision Z
        case 0x2000: revid = 'B'; break; // Revision B
        case 0x2001: revid = 'Y'; break; // Revision Y
        case 0x2003: revid = 'X'; break; // Revision X
        case 0x2007: revid = '1'; break; // Revision 1
        case 0x200F: revid = 'V'; break; // Revision V
        case 0x201F: revid = '2'; break; // Revision 2
    }
    // PTS_f(uDEBUG, "MCU: RevID = %c, DevID = 0x%X", revid, (val & 0xFFF));
    uint16_t len = snprintf(MSG, MSG_SIZE, "MCU: RevID = %c, DevID = 0x%lX\r\n", revid, (val & 0xFFF));
    uart_print(MSG, len);
}
