#include "adc_control.h"
#include "esp_log.h"

static const char *TAG = "adc_control.c";

void adc_init(void)
{
    // Configure ADC width (resolution)
    adc1_config_width(ADC_WIDTH);

    // Configure ADC channel
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN);
}

esp_err_t adc_read() {
    // Read ADC value (returns a value between 0 and 4095 for 12-bit resolution)
    int adc_value = adc1_get_raw(ADC_CHANNEL);
        
    // Calculate the input voltage (0-3.3V)
    float voltage = adc_value * (3.3f / 4095.0f);
    
    ESP_LOGI(TAG, "ADC Reading: %d, Voltage: %.2f V", adc_value, voltage);

    // Delay for a bit before reading again
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void mems_task(void *arg) {
    // Read from the sensor here
    adc_read();
    vTaskDelay(pdMS_TO_TICKS(10)); // Wait for 10 milliseconds
}