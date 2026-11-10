#include "bmp180.h"

static bmp180_read_t  bmp_read  = 0;
static bmp180_write_t bmp_write = 0;
static bmp180_delay_t bmp_delay = 0;

static int16_t  ac1, ac2, ac3, b1, b2, mb, mc, md;
static uint16_t ac4, ac5, ac6;

void bmp180_register_callbacks(bmp180_read_t read, bmp180_write_t write, bmp180_delay_t delay)
{
  bmp_read  = read;
  bmp_write = write;
  bmp_delay = delay;
}

uint8_t bmp180_init(void)
{
  uint8_t id;
  uint8_t val;
  uint8_t c[22];

  if (bmp_read == 0 || bmp_write == 0 || bmp_delay == 0) return 1;

  if (bmp_read(BMP180_ADDR, BMP180_REG_ID, &id, 1)) return 1;
  if (id != BMP180_ID_VALUE) return 2;

  val = BMP180_RESET_VALUE;
  if (bmp_write(BMP180_ADDR, BMP180_REG_RESET, &val, 1)) return 1;
  bmp_delay(10);

  if (bmp_read(BMP180_ADDR, BMP180_REG_CALIB, c, 22)) return 1;
  ac1 = (int16_t)((c[0] << 8) | c[1]);
  ac2 = (int16_t)((c[2] << 8) | c[3]);
  ac3 = (int16_t)((c[4] << 8) | c[5]);
  ac4 = (uint16_t)((c[6] << 8) | c[7]);
  ac5 = (uint16_t)((c[8] << 8) | c[9]);
  ac6 = (uint16_t)((c[10] << 8) | c[11]);
  b1  = (int16_t)((c[12] << 8) | c[13]);
  b2  = (int16_t)((c[14] << 8) | c[15]);
  mb  = (int16_t)((c[16] << 8) | c[17]);
  mc  = (int16_t)((c[18] << 8) | c[19]);
  md  = (int16_t)((c[20] << 8) | c[21]);

  return 0;
}
