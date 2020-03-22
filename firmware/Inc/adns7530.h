
#include <stdint.h>

typedef struct {
  int16_t x;
  int16_t y;
  uint8_t surface;
  uint8_t motion_flag;
} trackball_info_t;

// 16bitの2の補数に変換する
// 有効ビットは12だが符号ビットの関係で左詰めにする
static inline void to_tb_info(trackball_info_t* const tb_info, uint8_t dat1,
                              uint8_t dat2, uint8_t dat3) {
  tb_info->x = (int16_t)(((int16_t)dat1 << 4) | (((int16_t)dat3 >> 4) << 12));
  tb_info->y = (int16_t)(((int16_t)dat2 << 4) | (((int16_t)dat3 & 0x0F) << 12));
}

int adns7530_check_connection();
int adns7530_init(uint8_t laser_power);
int adns7530_read(trackball_info_t* const tb_info);
int adns7530_read_async(uint8_t* rcv, void (*callback)(void));
