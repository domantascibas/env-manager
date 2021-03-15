#include "init.h"
#include "gpio.h"
#include "uart.h"
#include "version.h"
#include "reset_source.h"
#include "task_manager.h"

void hw_init(void) {
    MX_GPIO_Init();
    uart_init();
}



void modules_init(void) {

}

    // /* else init. should start from task manager*/
    // // MX_ADC1_Init();
    // // MX_CRC_Init();
    // // MX_I2C1_Init();
    // // MX_RTC_Init();
    // // MX_SPI1_Init();
    // // // MX_WWDG_Init();
    // print_reset_source();
    // print_mcu_id_code();
    // print_version();
