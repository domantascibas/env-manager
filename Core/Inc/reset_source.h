#ifndef RESET_SOURCE_H_
#define RESET_SOURCE_H_

#include "stm32f103xb.h"
#include "stdint.h"

void reset_source_init(void);
uint8_t get_reset_source(void);
void print_mcu_id_code(void);

#endif
