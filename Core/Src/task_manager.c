#include "task_manager.h"
#include "uart.h"
#include "init.h"

#include "status_led.h"

static const char moduleStr[] = "TSK";
#define PTS_dbg(fmt, ...) PTS_d(moduleStr, fmt, ##__VA_ARGS__)
#define PTS_dbg_f(fmt, ...) PTS_df(moduleStr, fmt, ##__VA_ARGS__)

static xTaskHandle _hInit;
static xTaskHandle _hUartTx;
static xTaskHandle _hUartRx;
static xTaskHandle _hTaskManager;
static xTaskHandle _hStatusLed;
static xTaskHandle _hPrintBlink;

void _tTaskManager(void *argument);

static const taskDescription_t tasks[] = {
    {taskINIT,              "init",                 _tInit,             &_hInit,                NULL,       configMINIMAL_STACK_SIZE,           16,         1},
    {taskUartTx,            "UartTx",               _tUartTx,           &_hUartTx,              NULL,       configMINIMAL_STACK_SIZE * 2,       16,         1},
    {taskUartRx,            "UartRx",               _tUartRx,           &_hUartRx,              NULL,       configMINIMAL_STACK_SIZE * 2,       16,         1},
    {taskStatusLed,         "AliveLed",             _tStatusLed,        &_hStatusLed,           NULL,       configMINIMAL_STACK_SIZE,           16,         1},
    {taskTskManager,        "taskManager",          _tTaskManager,      &_hTaskManager,         NULL,       configMINIMAL_STACK_SIZE,           16,         1},
    {taskPrintBlink,        "printBlink",           _tPrintBlink,       &_hPrintBlink,          NULL,       configMINIMAL_STACK_SIZE,           16,         0},
};

int8_t getTaskIndex(eTaskID id) {
    uint8_t i;
    for (i = 0; i < SIZEOF(tasks); i++) {
        if (tasks[i].hardID == id) {
            return i;
        }
    }
    return -1;
}

const taskDescription_t* findTask(eTaskID id) {
    int8_t idx = getTaskIndex(id);
    if (idx >= 0) {
        return &tasks[idx];
    } else {
        PTS_dbg_f(" unknown task ID: %d", id);
    }
    return NULL;
}

void taskStart(eTaskID id) {
    const taskDescription_t *task = findTask(id);
    if (task != NULL) {
        PTS_dbg_f(" +%-16s START", task->name);
        xTaskCreate(task->func, task->name, task->stack_size, task->parameters, task->priority, task->handle);
    }
}

void taskStop(eTaskID id) {
    const taskDescription_t *task = findTask(id);
    if (task != NULL) {
        PTS_dbg_f(" -%-16s STOP", task->name);
        vTaskDelete(*task->handle);
    }
}

void _tTaskManager(void *argument) {
    // while(1) {
    //     /* print some task statistics */
    // }
    taskStop(taskTskManager);
}

void task_toggle(void) {
    static uint8_t state_en = 0;

    if (!state_en) {
        taskStart(taskPrintBlink);
        state_en = 1;
    } else {
        taskStop(taskPrintBlink);
        state_en = 0;
    }
}

