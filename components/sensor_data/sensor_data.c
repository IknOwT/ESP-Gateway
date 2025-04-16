#include <stddef.h>
#include "sensor_data.h"

sensor_data_t sensor_data = {0};             // Defines the global sensor data
SemaphoreHandle_t xSensorDataMutex = NULL;   // Defines the global mutex