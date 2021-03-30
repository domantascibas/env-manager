#include "version.h"
#include "uart.h"
#include "string.h"
#include "stdint.h"
#include "stdio.h"

#define RELEASE_VER             "00.0A.00.07"

const char version[VERSION_LENGTH] = RELEASE_VER;

void print_version(void) {
    PTS_f("VERSIONS:\r\n  Firmware version: %s", version);
}

void print_version_str(void) {
    PTS_f("%s", version);
}
