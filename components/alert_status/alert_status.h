#ifndef ALERT_STATUS_H
#define ALERT_STATUS_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

extern bool alert_active;
extern SemaphoreHandle_t xAlertMutex;

#endif // ALERT_STATUS_H