#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>
#include "pwm_control.h"
#include "i2c_control.h"
#include "adc_control.h"
#include "alarm_task.h"

extern SemaphoreHandle_t xSensorDataMutex;

void app_main(void)
{
    pwm_init();          //  Initialize the PWM
    buzzer_boot_tone();
    i2c_init();          //  Initialize I2C
    cont_adc_init();     //  Initialize the continuous ADC

    // 🛡️ Create mutex
    xSensorDataMutex = xSemaphoreCreateMutex();
    if (xSensorDataMutex == NULL) {
        ESP_LOGE("MAIN", "❌ Failed to create mutex!");
        return;
    }

    // 🚀 Start tasks
    xTaskCreate(scd41_task, "scd41_task", 4096, NULL, 5, NULL);
    xTaskCreate(mems_task, "mems_task", 4096, NULL, 5, NULL);
    xTaskCreate(alarm_task, "alarm_task", 2048, NULL, 4, NULL);
}