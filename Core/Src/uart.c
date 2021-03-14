#include "uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "commands.h"

#include "main.h"

static const char moduleStr[] = "UART";
#define PTS_dbg(fmt, ...) PTS_d(moduleStr, fmt, ##__VA_ARGS__)
#define PTS_dbg_f(fmt, ...) PTS_df(moduleStr, fmt, ##__VA_ARGS__)

#define UART_INTERFACE_COUNT        1
#define QUEUE_LENGTH                (MAX_MESSAGE_LENGTH * 8)
#define QUEUE_ITEM_SIZE             sizeof(uint8_t)
#define TX_SEMAPHORE_MAX_WAIT       1000
#define TX_QUEUE_MAX_WAIT           1000
#define CHAR_TX_TIMEOUT             5

static char MSG[MAX_MESSAGE_LENGTH];
UART_HandleTypeDef UartHandle;

static xTaskHandle _TxTask;
static xTaskHandle _RxTask;
static xSemaphoreHandle _TxMutex;
// static xSemaphoreHandle _RxMutex;
static StaticQueue_t _TxQueueBuffer;
static StaticQueue_t _RxQueueBuffer;
static xQueueHandle _TxQueue;
static xQueueHandle _RxQueue;

uint8_t TxBuffer[QUEUE_LENGTH * QUEUE_ITEM_SIZE];
uint8_t RxBuffer[QUEUE_LENGTH * QUEUE_ITEM_SIZE];
uint8_t CmdBuffer[MAX_MESSAGE_LENGTH * QUEUE_ITEM_SIZE];

void uart_HAL_init(void);
uint8_t uart_put_string(char *string);
void Uart_TxTask(void *arguments);
void Uart_RxTask(void *arguments);
void clear_RxBuffer(uint8_t *idx);
// static void sendDateTimeString(uint8_t uartNum);

uint8_t RX1_Char;

void USART3_IRQHandler(void) {
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    HAL_UART_IRQHandler(&UartHandle);
	HAL_UART_Receive_IT(&UartHandle, &RX1_Char, 1);
    xQueueSendFromISR(_RxQueue, &RX1_Char, &xHigherPriorityTaskWoken);

    if( xHigherPriorityTaskWoken ) {
        taskYIELD();
    }
}

void uart_init(void) {
    uart_HAL_init();
    _TxQueue = xQueueCreateStatic(QUEUE_LENGTH, QUEUE_ITEM_SIZE, &(TxBuffer[0]), &_TxQueueBuffer);
    _RxQueue = xQueueCreateStatic(QUEUE_LENGTH, QUEUE_ITEM_SIZE, &(RxBuffer[0]), &_RxQueueBuffer);
    _TxMutex = xSemaphoreCreateMutex();

    xTaskCreate(Uart_TxTask, "UART TX Task", 128 * 1, NULL, 16, &_TxTask);
    xTaskCreate(Uart_RxTask, "UART RX Task", 128 * 2, NULL, 16, &_RxTask);
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

    typedef enum {
        TX_INIT,
        TX_IDLE,
        TX_SENDING
    } txTaskState_t;

    txTaskState_t txTaskState = TX_INIT;
    HAL_UART_Receive_IT(&UartHandle, &RX1_Char, 1);
    char c;

    while (1) {
        switch (txTaskState) {
            case TX_INIT:
                // could allow UART to sleep - not yet implemented
                txTaskState = TX_IDLE;
                break;

            case TX_IDLE:
                xQueueReceive(_TxQueue, &c, portMAX_DELAY);
                // prevent uart from locking, or wake up, if needed

                txTaskState = TX_SENDING;
                break;

            case TX_SENDING:
                HAL_UART_Transmit(&UartHandle, (uint8_t *)&c, 1, HAL_MAX_DELAY);
                if (xQueueReceive(_TxQueue, &c, pdMS_TO_TICKS(CHAR_TX_TIMEOUT)) == pdFAIL) {
                    txTaskState = TX_IDLE;
                }
                break;
        }
        // vTaskDelay(1);
    }
}

void Uart_RxTask(void *arguments) {
    PTS_dbg("UART RX tsk start");

    typedef enum {
        RX_INIT,
        RX_IDLE,
        RX_RECEIVING
    } rxTaskState_t;

    rxTaskState_t rxTaskState = RX_INIT;
    uint8_t idx = 0;
    char c;

    while(1) {
        switch(rxTaskState) {
            case RX_INIT:
                rxTaskState = RX_IDLE;
                clear_RxBuffer(&idx);
                break;

            case RX_IDLE:
                xQueueReceive(_RxQueue, &c, portMAX_DELAY);
                rxTaskState = RX_RECEIVING;
                break;

            case RX_RECEIVING:
                CmdBuffer[idx] = c;
                HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9);
                idx++;
                if (xQueueReceive(_RxQueue, &c, pdMS_TO_TICKS(CHAR_TX_TIMEOUT)) == pdFAIL) {
                    rxTaskState = RX_IDLE;
                }

                if ((CmdBuffer[idx - 1] == '\r' && CmdBuffer[idx - 2] == '\n') || (CmdBuffer[idx - 1] == '\n' && CmdBuffer[idx - 2] == '\r')) {
                    memset(&CmdBuffer[idx - 2], '\0', 2);
                    idx-=2;
                    parse_cmd(CmdBuffer, idx);
                    clear_RxBuffer(&idx);
                }
                break;
        }
    }
}

void clear_RxBuffer(uint8_t *idx) {
    memset(CmdBuffer, '\0', sizeof(CmdBuffer));
    *idx = 0;
}

// void USART3_IRQHandler(void) {
    // HAL_UART_IRQHandler(&UartHandle);
    // portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    // if (USART_GetITStatus(USART3, USART_IT_TXE) == SET) {
    //     /* The interrupt was caused by the TX register becoming empty.  Are
    //     there any more characters to transmit? */
    //     if (xQueueReceiveFromISR(xCharsForTx, &cChar, &xHigherPriorityTaskWoken) == pdTRUE) {
    //         /* A character was retrieved from the queue so can be sent to the
    //         USART now. */
    //         USART_SendData(USART3, cChar);
    //     } else {
    //         USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
    //     }
    // }

    // if (HAL_UART_Receive_IT(&UartHandle, bufferRx, MAX_MESSAGE_LENGTH) == HAL_OK) {
        // xQueueSendFromISR(_TxQueue, bufferRx, &xHigherPriorityTaskWoken);
        // xQueueSend(_TxQueue, "cc\r\n", pdMS_TO_TICKS(TX_QUEUE_MAX_WAIT));
    // }
    // if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET) {
    //     /* A character has been received on the USART, send it to the Rx
    //     handler task. */
    // HAL_UART_Receive_IT(&UartHandle, bufferRx, 1);
    // HAL_UART_RxCpltCallback();
    // }

    // /* If sending or receiving from a queue has caused a task to unblock, and
    // the unblocked task has a priority equal to or higher than the currently
    // running task (the task this ISR interrupted), then xHigherPriorityTaskWoken
    // will have automatically been set to pdTRUE within the queue send or receive
    // function.  portEND_SWITCHING_ISR() will then ensure that this ISR returns
    // directly to the higher priority unblocked task. */
    // portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);

    // // Check to see if we just slept, if so, wake up peripherals
    // //! @todo Get rid of this
    // //if(PowerMgmt_AreWeSleeping() == TRUE)
    // //   PowerMgmt_WakeUp();

    // static portBASE_TYPE xHigherPriorityTaskWoken;
    // // Set to false on interrupt entry
    // xHigherPriorityTaskWoken = FALSE;

    // // Get received byte (lower 8-bits) and error info from UART (higher 8-bits) (total 16-bits)
    // do
    // {
    //  uint16_t byte = UartCpComms_GetByte();

    //  // Mask error info
    //  uint8_t status = (byte >> 8);

    //  // Check for error
    //  if(status == (UartCpComms_RX_STS_BREAK | UartCpComms_RX_STS_PAR_ERROR | UartCpComms_RX_STS_STOP_ERROR
    //      | UartCpComms_RX_STS_OVERRUN | UartCpComms_RX_STS_SOFT_BUFF_OVER))
    //  {
    //      // UART error has occured
    //      //Main_SetErrorLed();
    //      #if(configPRINT_DEBUG_UartCpComms == 1)
    //          if(status == UartCpComms_RX_STS_MRKSPC)
    //          {
    //              static char* msgErrorMarkOrSpaceWasReceivedInParityBit = "DEBUG_RX_INT: Error: Mark or space was received in parity bit.\r\n";
    //              UartDebug_PutString(msgErrorMarkOrSpaceWasReceivedInParityBit);
    //          }
    //          else if(status == UartCpComms_RX_STS_BREAK)
    //          {
    //              static char* msgBreakWasDetected = "DEBUG_RX_INT: Error: Break was detected.\r\n";
    //              UartDebug_PutString(msgBreakWasDetected);
    //          }
    //          else if(status == UartCpComms_RX_STS_PAR_ERROR)
    //          {
    //              static char* msgErorrParity = "DEBUG_RX_INT: Error: Parity error was detected.\r\n";
    //              UartDebug_PutString(msgErorrParity);
    //          }
    //          else if(status == UartCpComms_RX_STS_STOP_ERROR)
    //          {
    //              static char* msgErorrStop = "DEBUG_RX_INT: Error: Stop error was detected.\r\n";
    //              UartDebug_PutString(msgErorrStop);
    //          }
    //          else if(status == UartCpComms_RX_STS_OVERRUN)
    //          {
    //              static char* msgErrorFifoRxBufferOverrun = "DEBUG_RX_INT: Error: FIFO RX buffer was overrun.\r\n";
    //              UartDebug_PutString(msgErrorFifoRxBufferOverrun);
    //          }
    //          else if(status == UartCpComms_RX_STS_FIFO_NOTEMPTY)
    //          {
    //              static char* msgErrorRxBufferNotEmpty = "DEBUG_RX_INT: Error: RX buffer not empty.\r\n";
    //              UartDebug_PutString(msgErrorRxBufferNotEmpty);
    //          }
    //          else if(status == UartCpComms_RX_STS_ADDR_MATCH)
    //          {
    //              static char* msgErrorAddressMatch = "DEBUG_RX_INT: Error: Address match.\r\n";
    //              UartDebug_PutString(msgErrorAddressMatch);
    //          }
    //          else if(status == UartCpComms_RX_STS_SOFT_BUFF_OVER)
    //          {
    //              static char* msgErrorSoftwareBufferOverflowed = "DEBUG_RX_INT: Error: RX software buffer ovverflowed.\r\n";
    //              UartDebug_PutString(msgErrorSoftwareBufferOverflowed);
    //          }
    //      #endif
    //  }
    //  else
    //  {
    //      // Put byte in queue (ISR safe function)
    //      xQueueSendToBackFromISR(_xRxQueue, &byte, &xHigherPriorityTaskWoken);
    //  }
    // }
    // while((UartCpComms_ReadRxStatus() & UartCpComms_RX_STS_FIFO_NOTEMPTY) != 0x00);

    // // Force a context swicth if interrupt unblocked a task with a higher or equal priority
    // // to the currently running task
    // portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
// }

// void UartComms_GetChar(char* singleChar)
// {
//  xQueueReceive(_xRxQueue, singleChar, portMAX_DELAY);
// }

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
