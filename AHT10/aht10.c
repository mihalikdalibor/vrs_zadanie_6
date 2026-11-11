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

uint8_t aht10_init(void)
{
  uint8_t param[2] = {AHT10_INIT_PARAM, 0x00};
  uint8_t status;

  if (aht_write == 0 || aht_receive == 0 || aht_delay == 0) return 1;

  aht_delay(40);

  if (aht_write(AHT10_ADDR, AHT10_CMD_RESET, 0, 0)) return 1;
  aht_delay(20);

  if (aht_write(AHT10_ADDR, AHT10_CMD_INIT, param, 2)) return 1;
  aht_delay(10);

  if (aht_receive(AHT10_ADDR, &status, 1)) return 1;
  if (!(status & AHT10_STATUS_CAL)) return 2;

  return 0;
}
