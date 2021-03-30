#ifndef _TASK_MANAGER_H_
#define _TASK_MANAGER_H_

#include "FreeRTOS.h"
#include "task.h"

#define SIZEOF(arr) (sizeof(arr) / sizeof(arr[0]))

typedef enum {
    taskINIT = 0,       // init system - modules and other
    taskUartTx,
    taskUartRx,
    taskTskManager,
    taskStatusLed,
    taskPrintBlink,
    taskI2C,
    taskLCD,
} eTaskID;

typedef struct {
    eTaskID const hardID;
    const char * const name;
    TaskFunction_t func;
    TaskHandle_t * const handle;
    void * const parameters;
    const configSTACK_DEPTH_TYPE stack_size;
    UBaseType_t priority;
} taskDescription_t;

void taskStart(eTaskID id);
void taskStop(eTaskID id);
void task_toggle(void);

#endif
