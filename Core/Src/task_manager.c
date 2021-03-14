#include "FreeRTOS.h"
#include "task_manager.h"
#include "uart.h"

static const char moduleStr[] = "TSK";
#define PTS_dbg(fmt, ...) PTS_d(moduleStr, fmt, ##__VA_ARGS__)
#define PTS_dbg_f(fmt, ...) PTS_df(moduleStr, fmt, ##__VA_ARGS__)

