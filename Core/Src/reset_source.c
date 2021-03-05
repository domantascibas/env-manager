#include "reset_source.h"
#include "drv_reset_source.h"
#include "uart.h"

uint8_t reset_src;

void reset_source_init(void) {
    reset_src = get_mcu_reset_source();
}

uint8_t get_reset_source(void) {
    return reset_src;
}

// return 1 if power was off; 0 - if power was on
uint8_t RS_power_reset(void) {
    return (reset_src & POR_RS) ? 1 : 0;
}

void print_mcu_id_code(void) {
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
    PTS_f("MCU: RevID = %c, DevID = 0x%X", revid, (val & 0xFFF));
}
