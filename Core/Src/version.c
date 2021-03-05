#include "version.h"
#include "uart.h"
#include "string.h"
#include "stdint.h"
#include "stdio.h"

#define RELEASE_VER             "00.0a.00.01"

const char version[VERSION_LENGTH] = RELEASE_VER;

void print_version(void) {
    PTS_f("VERSIONS:\r\nFirmware version: %s\r\n", version);
}
