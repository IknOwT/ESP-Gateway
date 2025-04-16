#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "alarm_task.h"
#include "pwm_control.h"
#include "alert_status.h"

static const char *TAG = "alarm_task.c";

void alert_task(void *param) {
    xTaskCreate(led_alarm_pulse, "led_alert", 2048, NULL, 5, NULL);
    xTaskCreate(buzzer_alert_tone, "buzzer_alert", 2048, NULL, 5, NULL);                      
    vTaskDelay(pdMS_TO_TICKS(60000));             // delay 1 minute
    alert_active = false;                        // mark buzzer as inactive
    xSemaphoreGive(xAlertMutex);                 // unlock the buzzer mutex
    vTaskDelete(NULL);                            // delete this task
}

void alarm_task(void *arg) {

    xAlertMutex = xSemaphoreCreateMutex();

    while (1) {
        uint16_t co2 = 0;
        float temp = 0.0f;
        float humid = 0.0f;
        float noise = 0.0f;

        if (xSemaphoreTake(xSensorDataMutex, portMAX_DELAY)) {
            co2 = sensor_data.CO2;
            temp = sensor_data.temperature;
            humid = sensor_data.humidity;
            noise = sensor_data.noise_level;
            xSemaphoreGive(xSensorDataMutex);
        }

        bool alarm_triggered = false;

        // Check and react to thresholds
        if (co2 > CO2_THRESHOLD) {
            ESP_LOGW(TAG, "CO2 concentration too high! %uppm", co2);
            alarm_triggered = true;
        }

        if (temp > TEMP_THRESHOLD) {
            ESP_LOGW(TAG, "Temperature too high! %.2f°C", temp);
            alarm_triggered = true;
        }

        if (humid > HUMIDITY_THRESHOLD) {
            ESP_LOGW(TAG, "Humidity too high! %.2f%%", humid);
            alarm_triggered = true;
        }

        if (noise > NOISE_THRESHOLD) {
            ESP_LOGW(TAG, "Noise level too high! %.2f dB", noise);
            alarm_triggered = true;
        }

        if (alarm_triggered && !alert_active) {
            if (xSemaphoreTake(xAlertMutex, 0)) {
                alert_active = true;
                // Launch a one-time task to alert for 1 min and then release
                xTaskCreatePinnedToCore(
                    alert_task,
                    "alert_task",
                    2048,
                    NULL,
                    5,
                    NULL,
                    tskNO_AFFINITY
                );
            }
        }

        vTaskDelay(pdMS_TO_TICKS(2000));  // Check every 2 seconds
    }
}