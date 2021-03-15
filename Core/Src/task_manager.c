#include "task_manager.h"
#include "uart.h"

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
    // {taskINIT,              "taskInit",             _tInit,             &_hInit,                NULL,       configMINIMAL_STACK_SIZE,           16,         1},
    {taskUartTx,            "taskUartTx",           _tUartTx,           &_hUartTx,              NULL,       configMINIMAL_STACK_SIZE * 2,       16,         1},
    {taskUartRx,            "taskUartRx",           _tUartRx,           &_hUartRx,              NULL,       configMINIMAL_STACK_SIZE * 2,       16,         1},
    {taskTskManager,        "taskManager",          _tTaskManager,      &_hTaskManager,         NULL,       configMINIMAL_STACK_SIZE,           16,         1},
    {taskStatusLed,         "statusLed",            _tStatusLed,        &_hStatusLed,           NULL,       configMINIMAL_STACK_SIZE,           16,         1},
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
        xTaskCreate(task->func, task->name, task->stack_size, task->parameters, task->priority, task->handle);
        PTS_dbg_f(" +%-16s START", task->name);
    }
}

void taskStop(eTaskID id) {
    const taskDescription_t *task = findTask(id);
    if (task != NULL) {
        vTaskDelete(*task->handle);
        PTS_dbg_f(" -%-16s STOP", task->name);
    }
}

void _tTaskManager(void *argument) {
    taskStart(taskStatusLed);
    // uint8_t i;
    // const taskDescription_t* task;

    // for (i = 0; i < SIZEOF(tasks); i++) {
    //     task = &tasks[i];
    //     if (task->autorun) {
    //         xTaskCreate(task->func, task->name, task->stack_size, task->parameters, task->priority, task->handle);
    //         PTS_dbg_f(" +%-16s START", task->name);
    //     }
    // }

    // while(1) {
    //     /* print some task statistics */
    // }

    vTaskDelete(_hTaskManager);
}

void task_toggle(void) {
    const taskDescription_t* task = &tasks[1];
    // PTS_dbg_f(" %s-16s handle:%d", task->name, *task->handle);

    if (*task->handle == 0) {
        xTaskCreate(task->func, task->name, task->stack_size, task->parameters, task->priority, task->handle);
        PTS_dbg_f(" +%-16s START", task->name);
    } else {
        eTaskState state = eTaskGetState(*task->handle);
        if (state <= eSuspended) {
            vTaskDelete(*task->handle);
            PTS_dbg_f(" -%-16s STOP", task->name);
        } else {
            xTaskCreate(task->func, task->name, task->stack_size, task->parameters, task->priority, task->handle);
            PTS_dbg_f(" +%-16s START", task->name);
        }
    }
}

