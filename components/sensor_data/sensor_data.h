#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

typedef struct {
    uint16_t CO2;
    float temperature;
    float humidity;
    float noise_level;
} sensor_data_t;

extern sensor_data_t sensor_data;
extern SemaphoreHandle_t xSensorDataMutex;

#endif // SENSOR_DATA_H