#include "init.h"
#include "gpio.h"
#include "uart.h"
#include "version.h"
#include "reset_source.h"
#include "task_manager.h"

#include "adc.h"
#include "crc.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
// #include "wwdg.h"

void hw_init(void) {
    MX_GPIO_Init();
    uart_init();
    taskStart(taskStatusLed);

    print_reset_source();
    print_mcu_id_code();
    print_version();
}

void _tInit(void *arguments) {
    /* else init. should start from task manager*/
    MX_RTC_Init();
    MX_CRC_Init();
    MX_ADC1_Init();
    MX_I2C1_Init();
    // MX_SPI1_Init();
    // // MX_WWDG_Init();

    /* start fw tasks */
    taskStart(taskI2C);

    // uint32_t Vref = 0;
    // float Vdd = 0.0;

    // while(1) {
    //     HAL_ADC_Start_IT(&hadc1);
    //     if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK) {
    //         Vref = HAL_ADC_GetValue(&hadc1);
    //     }
    //     Vdd = 4095 * 1.2 / Vref;
    //     PTS_f("ADC1: %ld, %f", Vref, Vdd);
    //     vTaskDelay(1000);
    // }

    taskStop(taskINIT);
}
