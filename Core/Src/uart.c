#include "uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "stdarg.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

#include "main.h"

static const char moduleStr[] = "UART";
#define PTS_dbg(fmt, ...) PTS_d(moduleStr, fmt, ##__VA_ARGS__)
#define PTS_dbg_f(fmt, ...) PTS_df(moduleStr, fmt, ##__VA_ARGS__)

#define UART_INTERFACE_COUNT        1
#define MAX_MESSAGE_LENGTH          64
#define QUEUE_LENGTH                (MAX_MESSAGE_LENGTH * 8)
#define QUEUE_ITEM_SIZE             sizeof(uint8_t)
#define TX_SEMAPHORE_MAX_WAIT       1000
#define TX_QUEUE_MAX_WAIT           1000
#define CHAR_TX_TIMEOUT             5

static char MSG[MAX_MESSAGE_LENGTH];
UART_HandleTypeDef UartHandle;

static xTaskHandle _TxTask;
static xSemaphoreHandle _TxMutex;
static StaticQueue_t _TxQueueBuffer;
static StaticQueue_t _RxQueueBuffer;
static xQueueHandle _TxQueue;
static xQueueHandle _RxQueue;

uint8_t TxBuffer[QUEUE_LENGTH * QUEUE_ITEM_SIZE];
uint8_t RxBuffer[QUEUE_LENGTH * QUEUE_ITEM_SIZE];

void uart_HAL_init(void);
uint8_t uart_put_string(char *string);
void Uart_TxTask(void *arguments);
// static void sendDateTimeString(uint8_t uartNum);

void uart_init(void) {
    uart_HAL_init();
    _TxQueue = xQueueCreateStatic(QUEUE_LENGTH, QUEUE_ITEM_SIZE, &(TxBuffer[0]), &_TxQueueBuffer);
    _RxQueue = xQueueCreateStatic(QUEUE_LENGTH, QUEUE_ITEM_SIZE, &(RxBuffer[0]), &_RxQueueBuffer);
    _TxMutex = xSemaphoreCreateMutex();

    xTaskCreate(Uart_TxTask, "UART TX Task", 128 * 1, NULL, 16, &_TxTask);
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
    if (xSemaphoreTake(_TxMutex, pdMS_TO_TICKS(TX_SEMAPHORE_MAX_WAIT)) == pdFAIL) {
        // check if uart dbg is enabled
        HAL_UART_Transmit(&UartHandle, (uint8_t *)"UART semphr tmo\r\n", 18, HAL_MAX_DELAY);
        xSemaphoreGive(_TxMutex);
        return 0;
    }

    while (*string != '\0') {
        if (xQueueSend(_TxQueue, string, pdMS_TO_TICKS(TX_QUEUE_MAX_WAIT)) == pdPASS) {
            string++;
        } else {
            HAL_UART_Transmit(&UartHandle, (uint8_t *)"queue full\r\n", 14, HAL_MAX_DELAY);
        }
    }
    xQueueSend(_TxQueue, "\r", pdMS_TO_TICKS(TX_QUEUE_MAX_WAIT));
    xQueueSend(_TxQueue, "\n", pdMS_TO_TICKS(TX_QUEUE_MAX_WAIT));

    xSemaphoreGive(_TxMutex);
    return 1;
}

void Uart_TxTask(void *arguments) {
    PTS_dbg("UART TX tsk start");
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
                if (xQueueReceive(_TxQueue, &c, pdMS_TO_TICKS(CHAR_TX_TIMEOUT)) == pdFAIL) {
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

void PTS_d(const char *module, const char *format, ...) {
    va_list argptr;
    char *ptr = MSG;
    int size = MAX_MESSAGE_LENGTH;
    int nch;
    // sendDateTimeString(uDEBUG);
    nch = snprintf(ptr, size, "[%s]->", module);
    ptr += nch;
    size -= nch;
    va_start(argptr, format);
    vsnprintf(ptr, size, format, argptr);
    PTS(MSG);
    va_end(argptr);
}

// void PTS_t(int num, char *string) {
//     // send current date/time to UART/USB
//     sendDateTimeString(num);
//     // put rest of the string (we came here with) to the UART using PTS()
//     PTS(num, string);
// }

void PTS_f(const char *format, ...) {
    va_list argptr;
    va_start(argptr, format);
    vsnprintf(MSG, MAX_MESSAGE_LENGTH, format, argptr);
    PTS(MSG);
    va_end(argptr);
}

void PTS_df(const char *module, const char *format, ...) {
    va_list argptr;
    char *ptr = MSG;
    int size = MAX_MESSAGE_LENGTH;
    int nch;
    nch = snprintf(ptr, size, "[%s]->", module);
    ptr += nch;
    size -= nch;
    va_start(argptr, format);
    vsnprintf(ptr, size, format, argptr);
    PTS(MSG);
    va_end(argptr);
}

// void PTS_tf(int num, const char *format, ...) {
//     va_list argptr;
//     // send current date/time to UART/USB
//     sendDateTimeString(num);
//     // process va_list and send it to UART/USB using PTS()
//     va_start(argptr, format);
//     vsnprintf(MSG, MSG_size, format, argptr);
//     PTS(num, MSG);
//     va_end(argptr);
// }

// static void sendDateTimeString(uint8_t uartNum) {
//     static char dtBin[8], dtStr[25];
//     uint8_t i = 0;
//     // Make current date/time string
//     RTC_unix_to_time(0, 0, dtBin);
//     snprintf(dtStr, 25, "[%02d%02d.%02d.%02d %02d:%02d:%02d|%01d]-", dtBin[0], dtBin[1], dtBin[2], dtBin[3], dtBin[4], dtBin[5], dtBin[6], dtBin[7]);
//     // put this string to the USB
//     if (uartNum == uDEBUG && USB_debug == 1) {
//         VCP_len(dtStr, 24);
//     }
//     if (UART_debug == 1) {
//         rtt_print_no_crlf((uint8_t *)dtStr, 24);
//         // put this string to the UART
//         for (i = 0; i < 24; i++) {
//             TX_char(uartNum, dtStr[i]);
//         }
//     }
// }
