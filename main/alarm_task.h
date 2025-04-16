#ifndef ALARM_TASK_H
#define ALARM_TASK_H

#include "sensor_data.h"

// Define thresholds
#define CO2_THRESHOLD      1000
#define TEMP_THRESHOLD     35.0f
#define HUMIDITY_THRESHOLD 80.0f
#define NOISE_THRESHOLD    100.0f  // in dB

void alarm_task(void *arg);

#endif // ALARM_TASK_H
