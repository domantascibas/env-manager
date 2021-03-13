#include "commands.h"
#include "uart.h"

static const char moduleStr[] = "CMD";
#define PTS_dbg(fmt, ...) PTS_d(moduleStr, fmt, ##__VA_ARGS__)
#define PTS_dbg_f(fmt, ...) PTS_df(moduleStr, fmt, ##__VA_ARGS__)

void parse_cmd(uint8_t *cmd, uint16_t sz) {
    PTS_dbg(cmd);
}
