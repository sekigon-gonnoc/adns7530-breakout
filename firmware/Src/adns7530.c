
#include "adns7530.h"

#include <stdbool.h>

#include "debug_util.h"
#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_gpio.h"
#include "stm32l0xx_hal_spi.h"

#define WRITE_TO(addr) (0x80 | addr)
#define READ_FROM(addr) (addr)
#define CS_PIN_TB 4

static bool trackball_init_flag = false;
extern SPI_HandleTypeDef hspi1;

static inline void spim_start(uint8_t* tx_buf, uint16_t tx_len, uint8_t* rx_buf,
                              uint16_t rx_len, uint8_t cs_pin) {
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
  // if (HAL_SPI_TransmitReceive(&hspi1, tx_buf, rx_buf, tx_len, 100) == HAL_OK)
  // {
  if (HAL_SPI_TransmitReceive_DMA(&hspi1, tx_buf, rx_buf, tx_len) == HAL_OK) {
    while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY) {
      continue;
    }
  }
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}

int adns7530_check_connection() {
  int32_t retry = 1000;

  // read trackball id
  uint8_t snd[] = {0, 0};
  uint8_t rcv[2];
  do {
    spim_start(snd, 2, rcv, 2, CS_PIN_TB);
  } while (rcv[1] != 0x31 && retry--);

  if (retry > 0) {
    trackball_init_flag = true;
    return 0;
  } else {
    return 1;
  }
}

int adns7530_init(uint8_t laser_power) {
  uint8_t rcv[256];

  {
    uint8_t snd[] = {
        WRITE_TO(0x3A), 0x5a, 0, 0, WRITE_TO(0x2E), 0, 0, 0,
    };
    spim_start(snd, sizeof(snd), rcv, sizeof(snd), CS_PIN_TB);
  }

  {
    uint8_t snd[] = {
        READ_FROM(0x2), 0, READ_FROM(0x3), 0,
        READ_FROM(0x4), 0, READ_FROM(0x5), 0,
    };
    spim_start(snd, sizeof(snd), rcv, sizeof(snd), CS_PIN_TB);
  }

  {
    uint8_t snd[] = {
        WRITE_TO(0x3C), 0x27, WRITE_TO(0x22), 0x0A, WRITE_TO(0x21), 0x01,
        WRITE_TO(0x3C), 0x32, WRITE_TO(0x23), 0x20, WRITE_TO(0x3C), 0x05,
    };
    spim_start(snd, sizeof(snd), rcv, sizeof(snd), CS_PIN_TB);
  }

  {
    // clang-format off
        // set laser power packet
        uint8_t snd[] = {
            WRITE_TO(0x1A), 0x40,
            WRITE_TO(0x1F), (uint8_t)((~0x40) & 0xFF),
            WRITE_TO(0x1C), 0xFF,
            WRITE_TO(0x1D), 0,
        };
    // clang-format on

    snd[5] = laser_power;
    snd[7] = ~laser_power;

    spim_start(snd, sizeof(snd), rcv, sizeof(snd), CS_PIN_TB);
  }

  return 0;
}

int adns7530_read(trackball_info_t* const tb_info) {
  // trackball communication packet
  uint8_t snd[] = {0x02, 0, 0x03, 0, 0x04, 0, 0x05, 0, 0x06, 0};
  uint8_t rcv[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  spim_start(snd, 10, rcv, 10, CS_PIN_TB);

  to_tb_info(tb_info, rcv[3], rcv[5], rcv[7]);
  tb_info->surface = rcv[9];
  tb_info->motion_flag = rcv[1];

  return 0;
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef* hspi) {
  if (hspi == &hspi1) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
  }
  // DEBUG_PRINT("spi dma complete\n");
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef* hspi) {
  if (hspi == &hspi1) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
  }
  DEBUG_PRINT("spi error callback\n");
}

int adns7530_read_async(uint8_t* rcv, void (*callback)(void)) {
  // trackball communication packet
  static uint8_t snd[] = {0x42, 0, 0, 0, 0};

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
  if (HAL_SPI_TransmitReceive_DMA(&hspi1, snd, rcv, 5) != HAL_OK) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
  }

  return 0;
}
