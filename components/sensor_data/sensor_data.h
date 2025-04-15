#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include "freertos/semphr.h"

typedef struct {
    float temperature;
    float humidity;
    float noise_level;
} sensor_data_t;

extern sensor_data_t sensor_data;
extern SemaphoreHandle_t xSensorDataMutex;

#endif // SENSOR_DATA_H