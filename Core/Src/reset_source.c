#include "reset_source.h"
#include "drv_reset_source.h"

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
