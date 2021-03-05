#include "uart.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "stdarg.h"

#include "main.h"

#define UART_INTERFACE_COUNT        1
#define QUEUE_MAX_ITEMS             64
#define QUEUE_ITEM_SIZE             1
#define TX_SEMAPHORE_MAX_WAIT       1000
#define TX_QUEUE_MAX_WAIT           1000
#define MAX_MESSAGE_LENGTH          QUEUE_MAX_ITEMS
#define CHAR_TX_TIMEOUT             5

UART_HandleTypeDef UartHandle;

static xTaskHandle _TxTask;
static xQueueHandle _TxQueue;
static xQueueHandle _RxQueue;
static xSemaphoreHandle _TxMutex;

void uart_HAL_init(void);
uint8_t uart_put_string(char *string);
void Uart_TxTask(void *arguments);

void uart_init(void) {
    uart_HAL_init();
    _TxQueue = xQueueCreate(QUEUE_MAX_ITEMS, QUEUE_ITEM_SIZE);
    _RxQueue = xQueueCreate(QUEUE_MAX_ITEMS, QUEUE_ITEM_SIZE);
    _TxMutex = xSemaphoreCreateMutex();

    xTaskCreate(Uart_TxTask, "UART TX Task", 128, NULL, 1, &_TxTask);
    PTS("\r\n*** STARTUP ***");
}

void uart_HAL_init(void) {
    UartHandle.Instance = USART3;
    UartHandle.Init.BaudRate = 115200;
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits = UART_STOPBITS_1;
    UartHandle.Init.Parity = UART_PARITY_NONE;
    UartHandle.Init.Mode = UART_MODE_TX_RX;
    UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&UartHandle) != HAL_OK) {
        Error_Handler();
    }
}

uint8_t uart_put_string(char *string) {
    if (xSemaphoreTake(_TxMutex, TX_SEMAPHORE_MAX_WAIT / portTICK_RATE_MS) == pdFAIL) {
        // check if uart dbg is enabled
        PTS("UART semphr tmo");
        return 0;
    }

    while (*string != '\0') {
        xQueueSend(_TxQueue, string, TX_QUEUE_MAX_WAIT / portTICK_RATE_MS);
        string++;
    }
    xQueueSend(_TxQueue, "\r", TX_QUEUE_MAX_WAIT / portTICK_RATE_MS);
    xQueueSend(_TxQueue, "\n", TX_QUEUE_MAX_WAIT / portTICK_RATE_MS);

    xSemaphoreGive(_TxMutex);
    return 1;
}

void Uart_TxTask(void *arguments) {
    PTS("UART TX task started");
//                      start RX interrupt

    typedef enum {
        ST_INIT,
        ST_IDLE,
        ST_SENDING
    } txTaskState_t;

    txTaskState_t txTaskState = ST_INIT;

    while (1) {
        char c;

        switch (txTaskState) {
            case ST_INIT:
                // could allow UART to sleep - not yet implemented
                txTaskState = ST_IDLE;
                break;

            case ST_IDLE:
                xQueueReceive(_TxQueue, &c, portMAX_DELAY);
                // prevent uart from locking, or wake up, if needed

                txTaskState = ST_SENDING;
                break;

            case ST_SENDING:
                HAL_UART_Transmit(&UartHandle, (uint8_t *)&c, 1, HAL_MAX_DELAY);
                if (xQueueReceive(_TxQueue, &c, CHAR_TX_TIMEOUT / portTICK_RATE_MS) == pdFAIL) {
                    txTaskState = ST_IDLE;
                }
                break;
        }
        // vTaskDelay(1);
    }
}

void PTS(char *string) {
    uart_put_string(string);
}

static char MSG[MAX_MESSAGE_LENGTH];

void PTS_f(const char *format, ...) {
    va_list argptr;
    va_start(argptr, format);
    vsnprintf(MSG, MAX_MESSAGE_LENGTH, format, argptr);
    PTS(MSG);
    va_end(argptr);
}

// int UART_rxByte(int uartNo) {
//     if (uartNo >= UART_INTERFACE_CNT) return -1;
//     return Queue_popByte(uartData[uartNo].rxQueue);
// }
