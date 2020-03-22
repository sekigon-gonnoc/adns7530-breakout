
#include "user.h"

#include <stdbool.h>

#include "i2c_interface.h"
#include "adns7530.h"
#include "debug_util.h"

#include "stm32l0xx_hal.h"

static bool adns7530_init_flag = false;

void user_init() {
  if (adns7530_check_connection() == 0) {
    adns7530_init(0x7F);
    adns7530_init_flag = true;
    DEBUG_PRINT("ADNS-7530 found.\n");
  } else {
    DEBUG_PRINT("ADNS-7530 not found.\n");
  }
}

void user_main() {
  uint8_t raw[5] = {0};
  trackball_info_t tb_info;

  while (1) {
    DEBUG_PRINT("wait i2c\n");
    i2c_set_buffer(&raw[0], 5);
    i2c_wait_cmd();
    DEBUG_PRINT("wakeup\n");

    if (adns7530_init_flag) {
      adns7530_read_async(raw, NULL);
      to_tb_info(&tb_info, raw[2], raw[3], raw[4]);
      tb_info.motion_flag = raw[1];
      DEBUG_PRINT("0x%02x %5d %5d\n", tb_info.motion_flag, tb_info.x,
                  tb_info.y);
    } else {
      raw[0] = 0x00;
      raw[1] = 0x00;
      raw[2] = 0xDE;
      raw[3] = 0xAD;
      raw[4] = 0x00;
    }
  }
}
