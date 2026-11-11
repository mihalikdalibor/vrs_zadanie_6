#ifndef BMP180_H
#define BMP180_H

#include <stdint.h>

#define BMP180_ADDR          0xEE

#define BMP180_REG_CALIB     0xAA
#define BMP180_REG_ID        0xD0
#define BMP180_REG_RESET     0xE0
#define BMP180_REG_CTRL      0xF4
#define BMP180_REG_OUT       0xF6

#define BMP180_ID_VALUE      0x55
#define BMP180_RESET_VALUE   0xB6
#define BMP180_CMD_TEMP      0x2E
#define BMP180_CMD_PRESS     0x34

#define BMP180_OSS           3
#define BMP180_PRESS_DELAY   26
#define BMP180_REF_SAMPLES   10

typedef uint8_t (*bmp180_read_t)(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint8_t len);
typedef uint8_t (*bmp180_write_t)(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint8_t len);
typedef void    (*bmp180_delay_t)(uint32_t ms);

void bmp180_register_callbacks(bmp180_read_t read, bmp180_write_t write, bmp180_delay_t delay);
uint8_t bmp180_init(void);
uint8_t bmp180_get_temperature(float *temp);
uint8_t bmp180_get_pressure(float *press);
uint8_t bmp180_get_ref_pressure(float *ref);
float bmp180_get_altitude(float press, float ref_press);

#endif
