#ifndef AHT10_H
#define AHT10_H

#include <stdint.h>

#define AHT10_ADDR            0x70

#define AHT10_CMD_INIT        0xE1
#define AHT10_CMD_MEASURE     0xAC
#define AHT10_CMD_RESET       0xBA

#define AHT10_INIT_PARAM      0x08
#define AHT10_MEASURE_PARAM   0x33

#define AHT10_STATUS_BUSY     0x80
#define AHT10_STATUS_CAL      0x08
#define AHT10_MEASURE_DELAY   80

typedef uint8_t (*aht10_write_t)(uint8_t dev_addr, uint8_t cmd, uint8_t *data, uint8_t len);
typedef uint8_t (*aht10_receive_t)(uint8_t dev_addr, uint8_t *data, uint8_t len);
typedef void    (*aht10_delay_t)(uint32_t ms);

void aht10_register_callbacks(aht10_write_t write, aht10_receive_t receive, aht10_delay_t delay);
uint8_t aht10_init(void);

uint8_t aht10_get_humidity(float *hum);
uint8_t aht10_get_temperature(float *temp);

#endif
