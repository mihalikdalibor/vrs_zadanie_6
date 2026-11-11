#include "aht10.h"

static aht10_write_t   aht_write   = 0;
static aht10_receive_t aht_receive = 0;
static aht10_delay_t   aht_delay   = 0;

void aht10_register_callbacks(aht10_write_t write, aht10_receive_t receive, aht10_delay_t delay)
{
  aht_write   = write;
  aht_receive = receive;
  aht_delay   = delay;
}

uint8_t aht10_write_cmd(uint8_t cmd, uint8_t *param, uint8_t len)
{
  return aht_write(AHT10_ADDR, cmd, param, len);
}

uint8_t aht10_read(uint8_t *data, uint8_t len)
{
  return aht_receive(AHT10_ADDR, data, len);
}

uint8_t aht10_init(void)
{
  uint8_t param[2] = {AHT10_INIT_PARAM, 0x00};
  uint8_t status;

  if (aht_write == 0 || aht_receive == 0 || aht_delay == 0) return 1;

  aht_delay(40);

  if (aht10_write_cmd(AHT10_CMD_RESET, 0, 0)) return 1;
  aht_delay(20);

  if (aht10_write_cmd(AHT10_CMD_INIT, param, 2)) return 1;
  aht_delay(10);

  if (aht10_read(&status, 1)) return 1;
  if (!(status & AHT10_STATUS_CAL)) return 2;

  return 0;
}

static uint8_t aht10_measure(uint8_t *d)
{
  uint8_t param[2] = {AHT10_MEASURE_PARAM, 0x00};

  if (aht10_write_cmd(AHT10_CMD_MEASURE, param, 2)) return 1;
  aht_delay(AHT10_MEASURE_DELAY);
  if (aht10_read(d, 6)) return 1;
  if (d[0] & AHT10_STATUS_BUSY) return 1;

  return 0;
}

uint8_t aht10_get_humidity(float *hum)
{
  uint8_t d[6];
  uint32_t raw;

  if (aht10_measure(d)) return 1;

  raw = ((uint32_t)d[1] << 12) | ((uint32_t)d[2] << 4) | (d[3] >> 4);
  *hum = raw * 100.0f / 1048576.0f;
  return 0;
}

uint8_t aht10_get_temperature(float *temp)
{
  uint8_t d[6];
  uint32_t raw;

  if (aht10_measure(d)) return 1;

  raw = ((uint32_t)(d[3] & 0x0F) << 16) | ((uint32_t)d[4] << 8) | d[5];
  *temp = raw * 200.0f / 1048576.0f - 50.0f;
  return 0;
}
