#include "drv_reset_source.h"

#include "stdio.h"

uint8_t get_mcu_reset_source(void) {
    uint8_t rs = 0;
    // read pin reset flag
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST)) {
        rs |= PIN_RS;
    }
    // read POR flag
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST)) {
        rs |= POR_RS;
    }
    // read RCC_FLAG_SFTRST flag
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)) {
        rs |= SFT_RS;
    }
    // read RCC_FLAG_IWDGRST flag
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST)) {
        rs |= IWDG_RS;
    }
    // read RCC_FLAG_WWDGRST flag
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST)) {
        rs |= WWDG_RS;
    }
    // read RCC_FLAG_LPWRRST flag
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST)) {
        rs |= LPWR_RS;
    }
    __HAL_RCC_CLEAR_RESET_FLAGS();

    return rs;
}
