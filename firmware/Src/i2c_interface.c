
#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_i2c.h"
#include "stm32l0xx_hal_i2c_ex.h"
#include "stm32l0xx_hal_pwr.h"
#include "stm32l0xx_hal_pwr_ex.h"

#include "debug_util.h"

extern I2C_HandleTypeDef hi2c1;


void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c) {
    DEBUG_PRINT("i2c cplt:%d\n", hi2c->State);
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
    DEBUG_PRINT("i2c error cb\n");
    NVIC_SystemReset();
}

void i2c_wait_cmd() {
  HAL_SuspendTick();
  // while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {
    DEBUG_PRINT(".");
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
  // }
  HAL_ResumeTick();
}

int i2c_set_buffer(uint8_t * buf, uint16_t len) {
  if (HAL_I2C_Slave_Transmit_DMA(&hi2c1, buf, len) != HAL_OK) {
    DEBUG_PRINT("i2c error\n");
    NVIC_SystemReset();
    return 1;
  }

  return 0;
}
