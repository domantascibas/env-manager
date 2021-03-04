#ifndef RESET_SOURCE_H_
#define RESET_SOURCE_H_

#include "stm32f103xb.h"
#include "stdint.h"

uint8_t get_reset_source(void);
void reset_source_init(void);

#endif
