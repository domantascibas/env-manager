#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "iwdg.h"
#include "queue.h"
#include "usart.h"

#include "string.h"
#include "stdio.h"

#define TXRX_BUFFER_SIZE 128

osThreadId_t statusLedHandle;
osThreadId_t iwdgResetHandle;

/**************** QUEUE HANDLER *****************/
xQueueHandle St_Queue_Handler;

/**************** TASK HANDLER *****************/
xTaskHandle Sender1_Task_Handler;
xTaskHandle Sender2_Task_Handler;
xTaskHandle Receiver_Task_Handler;

/*************** TASK FUNCTIONS ****************/
void Sender1_Task(void *argument);
void Sender2_Task(void *argument);
void Receiver_Task(void *argument);

/**************** STRUCTURE DEFINITION *****************/

typedef struct {
    char *str;
    int counter;
    uint16_t large_value;
} my_struct;


int indx1 = 0;
int indx2 = 0;

const osThreadAttr_t statusLed_attributes = {
    .name = "statusLed",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityNormal,
};

const osThreadAttr_t iwdgReset_attributes = {
    .name = "wdgReset",
    .stack_size = 128,
    .priority = (osPriority_t) osPriorityHigh,
};

void statusLedTask(void *argument);
void iwdgResetTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

void MX_FREERTOS_Init(void) {
    iwdgResetHandle = osThreadNew(iwdgResetTask, NULL, &iwdgReset_attributes);
    statusLedHandle = osThreadNew(statusLedTask, NULL, &statusLed_attributes);

    St_Queue_Handler = xQueueCreate(2, sizeof(my_struct));

    if (St_Queue_Handler == 0) { // if there is some error while creating queue
        char *str = "Unable to create STRUCTURE Queue\r\n";
        HAL_UART_Transmit(&huart3, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
    } else {
        char *str = "STRUCTURE Queue Created successfully\r\n";
        HAL_UART_Transmit(&huart3, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
    }

    xTaskCreate(Sender1_Task, "SENDER1", TXRX_BUFFER_SIZE, NULL, 2, &Sender1_Task_Handler);
    xTaskCreate(Sender2_Task, "SENDER2", TXRX_BUFFER_SIZE, NULL, 2, &Sender2_Task_Handler);
    xTaskCreate(Receiver_Task, "RECEIVER", TXRX_BUFFER_SIZE, NULL, 1, &Receiver_Task_Handler);
}

void statusLedTask(void *argument) {
    /* USER CODE BEGIN startStatusLed */
    /* Infinite loop */
    while(1) {
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_8);
        osDelay(100);
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9);
        osDelay(100);
    }
    /* USER CODE END startStatusLed */
}

void iwdgResetTask(void *argument) {
    while(1) {
        osDelay(4500);
        iwdgReset();
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
        osDelay(100);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
    }
}

void Sender1_Task(void *argument) {
    my_struct *ptrtostruct;

    uint32_t TickDelay = pdMS_TO_TICKS(2000);
    while (1) {
        char *str = "Entered SENDER1_Task\r\n about to SEND to the queue\r\n";
        HAL_UART_Transmit(&huart3, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);

        /****** ALOOCATE MEMORY TO THE PTR ********/
        ptrtostruct = pvPortMalloc(sizeof(my_struct));

        /********** LOAD THE DATA ***********/
        ptrtostruct->counter = 1 + indx1;
        ptrtostruct->large_value = 1000 + indx1 * 100;
        ptrtostruct->str = "HELLO FROM SENDER 1 ";

        /***** send to the queue ****/
        if (xQueueSend(St_Queue_Handler, &ptrtostruct, portMAX_DELAY) == pdPASS) {
            char *str2 = " Successfully sent the to the queue\r\nLeaving SENDER1_Task\r\n\n";
            HAL_UART_Transmit(&huart3, (uint8_t *)str2, strlen(str2), HAL_MAX_DELAY);
        }

        indx1 = indx1 + 1;

        vTaskDelay(TickDelay);
    }
}


void Sender2_Task(void *argument) {
    my_struct *ptrtostruct;

    uint32_t TickDelay = pdMS_TO_TICKS(2000);
    while (1) {
        char *str = "Entered SENDER2 Task\r\n about to SEND to the queue\r\n";
        HAL_UART_Transmit(&huart3, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);

        /**** Allocate memory for the structure ****/
        ptrtostruct = pvPortMalloc(sizeof(my_struct));

        /**** Load the data into the Structure ****/
        ptrtostruct->str = "Sender2 says Hii!!!";
        ptrtostruct->large_value = 2000 + 200 * indx2;
        ptrtostruct->counter = 1 + indx2;
        if (xQueueSend(St_Queue_Handler, &ptrtostruct, portMAX_DELAY) == pdPASS) {
            char *str2 = " Successfully sent the to the queue\r\nLeaving SENDER2_Task\r\n\n";
            HAL_UART_Transmit(&huart3, (uint8_t *)str2, strlen(str2), HAL_MAX_DELAY);
        }

        indx2 = indx2 + 1;

        vTaskDelay(TickDelay);
    }

}

void Receiver_Task(void *argument) {
    my_struct *Rptrtostruct;
    uint32_t TickDelay = pdMS_TO_TICKS(3000);
    char *ptr;

    while (1) {
        char *str = "Entered RECEIVER Task\r\n about to RECEIVE FROM the queue\r\n";
        HAL_UART_Transmit(&huart3, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);

        /**** RECEIVE FROM QUEUE *****/
        if (xQueueReceive(St_Queue_Handler, &Rptrtostruct, portMAX_DELAY) == pdPASS) {
            ptr = pvPortMalloc(100 * sizeof(char));  // allocate memory for the string

            sprintf(ptr, "Received from QUEUE:\r\n COUNTER = %d\r\n LARGE VALUE = %u\r\n STRING = %s\r\n\n", Rptrtostruct->counter, Rptrtostruct->large_value, Rptrtostruct->str);
            HAL_UART_Transmit(&huart3, (uint8_t *)ptr, strlen(ptr), HAL_MAX_DELAY);

            vPortFree(ptr);  // free the string memory
        }

        vPortFree(Rptrtostruct);  // free the structure memory

        vTaskDelay(TickDelay);
    }
}
