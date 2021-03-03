#include "uart.h"
#include "stdint.h"
#include "string.h"

#include "main.h"

UART_HandleTypeDef uDebug;

char MSG[MSG_SIZE];

void uart_init(void) {
    uDebug.Instance = uDEBUG;
    uDebug.Init.BaudRate = 115200;
    uDebug.Init.WordLength = UART_WORDLENGTH_8B;
    uDebug.Init.StopBits = UART_STOPBITS_1;
    uDebug.Init.Parity = UART_PARITY_NONE;
    uDebug.Init.Mode = UART_MODE_TX_RX;
    uDebug.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uDebug.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&uDebug) != HAL_OK) {
        Error_Handler();
    }
    HAL_UART_Transmit(&uDebug, (uint8_t *)"UART init\r\n", 12, HAL_MAX_DELAY);
}

void uart_print(char *msg, uint16_t len) {
    HAL_UART_Transmit(&uDebug, (uint8_t *)msg, len, HAL_MAX_DELAY);
}

void TX_byte(void) {
    // HAL_UART_Transmit();
}
