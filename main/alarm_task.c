#include "freertos/task.h"
#include <stdio.h>
#include "alarm_task.h"
#include "esp_log.h"
#include <stdarg.h>


static const char *TAG = "alarm_task.c";

// Define thresholds
#define TEMP_THRESHOLD     35.0f
#define HUMIDITY_THRESHOLD 80.0f
#define NOISE_THRESHOLD    65.0f  // in dB

void alarm_task(void *arg) {
    while (1) {
        float temp = 0.0f, humid = 0.0f, noise = 0.0f;

        if (xSemaphoreTake(xSensorDataMutex, portMAX_DELAY)) {
            temp = sensor_data.temperature;
            humid = sensor_data.humidity;
            noise = sensor_data.noise_level;
            xSemaphoreGive(xSensorDataMutex);
        }

        // 💡 Check and react to thresholds
        if (temp > TEMP_THRESHOLD) {
            ESP_LOGW(TAG, "🔥 Temperature too high! %.2f°C", temp);
            // trigger_alarm_action("Temperature");
        }

        if (humid > HUMIDITY_THRESHOLD) {
            ESP_LOGW(TAG, "💦 Humidity too high! %.2f%%", humid);
            // trigger_alarm_action("Humidity");
        }

        if (noise > NOISE_THRESHOLD) {
            ESP_LOGW(TAG, "🔊 Noise level too high! %.2f dB", noise);
            // trigger_alarm_action("Noise");
        }

        vTaskDelay(pdMS_TO_TICKS(2000));  // Check every 2 seconds
    }
}