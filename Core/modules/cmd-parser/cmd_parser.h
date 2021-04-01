#ifndef _CMD_PARSER_H_
#define _CMD_PARSER_H_

#include "stdint.h"

typedef void(*functionPointerType)(void);
typedef struct {
    char const *name;
    functionPointerType execute;
    char const *help;
} CommandStruct_t;

void parse_cmd(uint8_t *cmd, uint16_t sz);

#endif
