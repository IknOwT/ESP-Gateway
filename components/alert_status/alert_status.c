#include <stddef.h>
#include "alert_status.h"

bool alert_active = false;
SemaphoreHandle_t xAlertMutex = NULL;