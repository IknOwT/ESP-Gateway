#ifndef I2C_CONTROL_H
#define I2C_CONTROL_H

#include "driver/i2c.h"
#include "sensor_data.h"

#define I2C_MASTER_SCL_IO    22      // Set SCL GPIO
#define I2C_MASTER_SDA_IO    21      // Set SDA GPIO
#define I2C_MASTER_FREQ_HZ   100000  // 400kHz I2C frequency
#define I2C_MASTER_NUM       I2C_NUM_0  // I2C port number
#define I2C_MASTER_TIMEOUT   1000 / portTICK_PERIOD_MS  

#define I2C_SCD4x_ADDR       0x62 //SCD41 Address
#define SCD4x_CMD_POWER_DOWN 0x36e0  // Power Down Command
#define SCD4x_CMD_WAKE_UP    0x36f6  // Wake up Command
#define SCD4x_CMD_MEAS       0x219d  // Single shot measurement command
#define SCD4x_CMD_READ       0xeC05  //Read data from sensor

void i2c_init(void);
void scd41_task(void *arg);

#endif