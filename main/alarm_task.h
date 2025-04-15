#ifndef ALARM_TASK_H
#define ALARM_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "sensor_data.h"

void alarm_task(void *arg);

#endif // ALARM_TASK_H
