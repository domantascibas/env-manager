#include "version.h"
#include "uart.h"
#include "string.h"
#include "stdint.h"
#include "stdio.h"

#define RELEASE_VER             "00.0a.00.01"

const char version[VERSION_LENGTH] = RELEASE_VER;

void print_version(void) {
    uint16_t len = snprintf(MSG, MSG_SIZE, "VERSIONS:\r\nFirmware version: %s\r\n", version);
    uart_print(MSG, len);
}
