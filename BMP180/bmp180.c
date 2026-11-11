#include "bmp180.h"
#include <math.h>

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

uint8_t bmp180_read_reg(uint8_t reg, uint8_t *data, uint8_t len)
{
  return bmp_read(BMP180_ADDR, reg, data, len);
}

uint8_t bmp180_write_reg(uint8_t reg, uint8_t value)
{
  return bmp_write(BMP180_ADDR, reg, &value, 1);
}

uint8_t bmp180_init(void)
{
  uint8_t id;
  uint8_t c[22];

  if (bmp_read == 0 || bmp_write == 0 || bmp_delay == 0) return 1;

  if (bmp180_read_reg(BMP180_REG_ID, &id, 1)) return 1;
  if (id != BMP180_ID_VALUE) return 2;

  if (bmp180_write_reg(BMP180_REG_RESET, BMP180_RESET_VALUE)) return 1;
  bmp_delay(10);

  if (bmp180_read_reg(BMP180_REG_CALIB, c, 22)) return 1;
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

static uint8_t bmp180_read_ut(int32_t *ut)
{
  uint8_t d[2];

  if (bmp180_write_reg(BMP180_REG_CTRL, BMP180_CMD_TEMP)) return 1;
  bmp_delay(5);
  if (bmp180_read_reg(BMP180_REG_OUT, d, 2)) return 1;

  *ut = ((int32_t)d[0] << 8) | d[1];
  return 0;
}

static uint8_t bmp180_read_up(int32_t *up)
{
  uint8_t d[3];

  if (bmp180_write_reg(BMP180_REG_CTRL, BMP180_CMD_PRESS + (BMP180_OSS << 6))) return 1;
  bmp_delay(BMP180_PRESS_DELAY);
  if (bmp180_read_reg(BMP180_REG_OUT, d, 3)) return 1;

  *up = (((int32_t)d[0] << 16) | ((int32_t)d[1] << 8) | d[2]) >> (8 - BMP180_OSS);
  return 0;
}

static int32_t bmp180_calc_b5(int32_t ut)
{
  int32_t x1, x2;

  x1 = ((ut - ac6) * ac5) >> 15;
  x2 = ((int32_t)mc << 11) / (x1 + md);
  return x1 + x2;
}

uint8_t bmp180_get_temperature(float *temp)
{
  int32_t ut, b5;

  if (bmp180_read_ut(&ut)) return 1;

  b5 = bmp180_calc_b5(ut);
  *temp = ((b5 + 8) >> 4) / 10.0f;
  return 0;
}

uint8_t bmp180_get_pressure(float *press)
{
  int32_t ut, up, b3, b5, b6, x1, x2, x3, p;
  uint32_t b4, b7;

  if (bmp180_read_ut(&ut)) return 1;
  if (bmp180_read_up(&up)) return 1;

  b5 = bmp180_calc_b5(ut);
  b6 = b5 - 4000;
  x1 = (b2 * ((b6 * b6) >> 12)) >> 11;
  x2 = (ac2 * b6) >> 11;
  x3 = x1 + x2;
  b3 = ((((int32_t)ac1 * 4 + x3) << BMP180_OSS) + 2) / 4;
  x1 = (ac3 * b6) >> 13;
  x2 = (b1 * ((b6 * b6) >> 12)) >> 16;
  x3 = ((x1 + x2) + 2) >> 2;
  b4 = (ac4 * (uint32_t)(x3 + 32768)) >> 15;
  b7 = ((uint32_t)up - b3) * (50000 >> BMP180_OSS);

  if (b7 < 0x80000000)
    p = (b7 * 2) / b4;
  else
    p = (b7 / b4) * 2;

  x1 = (p >> 8) * (p >> 8);
  x1 = (x1 * 3038) >> 16;
  x2 = (-7357 * p) >> 16;
  p = p + ((x1 + x2 + 3791) >> 4);

  *press = p / 100.0f;
  return 0;
}

uint8_t bmp180_get_ref_pressure(float *ref)
{
  float p;
  float sum = 0;

  for (uint8_t i = 0; i < BMP180_REF_SAMPLES; i++)
  {
    if (bmp180_get_pressure(&p)) return 1;
    sum += p;
  }

  *ref = sum / BMP180_REF_SAMPLES;
  return 0;
}

float bmp180_get_altitude(float press, float ref_press)
{
  return 44330.0f * (1.0f - powf(press / ref_press, 1.0f / 5.255f));
}
