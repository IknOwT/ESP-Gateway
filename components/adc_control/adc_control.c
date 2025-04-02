#include "adc_control.h"

adc_continuous_handle_t adc_cont_handle = NULL;

static const char *TAG = "adc_control.c";

void cont_adc_init(void) {
    adc_continuous_handle_t adc_handle;

    adc_digi_pattern_config_t adc_pattern[1];
    adc_pattern[0].atten = ADC_ATTEN_DB_12;
    adc_pattern[0].bit_width = ADC_BITWIDTH_12;
    adc_pattern[0].channel = ADC_CHANNEL_5;
    adc_pattern[0].unit = ADC_UNIT_1;

    adc_continuous_config_t adc_cfg = {
        .sample_freq_hz = ADC_SAMPLING,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
        .format = ADC_DIGI_OUTPUT_FORMAT_TYPE1,
        .pattern_num = 1,
        .adc_pattern = adc_pattern,
    };
    
    adc_continuous_handle_cfg_t adc_conf = {
        .max_store_buf_size = ADC_BUF_SIZE,   
        .conv_frame_size = CONV_FRAME_SIZE,
    };

    esp_err_t ret = adc_continuous_new_handle(&adc_conf, &adc_handle);
    if (ret != ESP_OK) {
        ESP_LOGI(TAG, "Failed to create ADC continuous handle: %s", esp_err_to_name(ret));
    }

    ret = adc_continuous_config(adc_handle, &adc_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure ADC continuous mode: %s", esp_err_to_name(ret));
        return; // Exit if configuration fails
    }

    adc_cont_handle = adc_handle;
    ESP_LOGI(TAG, "ADC continuous mode initialized successfully");
}

void adc_cont_read(void) {
    // Allocate ADC buffer dynamically to prevent stack overflow
    uint8_t *adc_buffer = (uint8_t *)malloc(ADC_BUF_SIZE);
    if (adc_buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for ADC buffer");
        return;
    }

    // Start ADC continuous mode
    esp_err_t ret = adc_continuous_start(adc_cont_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start ADC continuous mode: %s", esp_err_to_name(ret));
        free(adc_buffer);
        return;
    }

    // Read ADC data into buffer
    uint32_t bytes_read = 0;
    ret = adc_continuous_read(adc_cont_handle, adc_buffer, ADC_BUF_SIZE, &bytes_read, portMAX_DELAY);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read ADC data: %s", esp_err_to_name(ret));
        free(adc_buffer);
        return;
    }

    // Verify that we read exactly one frame (1024 samples, 2048 bytes)
    if (bytes_read != ADC_BUF_SIZE) {
        ESP_LOGE(TAG, "Unexpected number of bytes read: %" PRIu32, bytes_read);
        free(adc_buffer);
        return;
    }

    // Process the frame: Compute the average ADC value
    uint32_t adc_sum = 0;
    for (int i = 0; i < (ADC_BUF_SIZE / 2); i++) {
        int16_t adc_sample = ((adc_buffer[i * 2] | (adc_buffer[i * 2 + 1] << 8)) & 0x0FFF);  // Extract the upper 12 bits (ADC data), discard lower 4 bits (channel)
        adc_sum += adc_sample;
    }

    // Compute the average ADC value
    uint32_t adc_avg = adc_sum / (ADC_BUF_SIZE / 2);

    // Print raw ADC values for debugging
    ESP_LOGI(TAG, "ADC Avg Raw Value (12-bit): %" PRIu32, adc_avg);

    // Convert ADC value to actual voltage
    float voltage = ((float)adc_avg / (float)ADC_MAX_VALUE) * ADC_REF_VOLTAGE;

    // Print the averaged voltage level
    ESP_LOGI(TAG, "Average ADC Voltage: %.3f V", voltage);

    // Stop ADC continuous mode after reading the frame
    ret = adc_continuous_stop(adc_cont_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to stop ADC continuous mode: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Successfully read and processed one frame of ADC data.");
    }

    // Free allocated buffer memory
    free(adc_buffer);
}

void mems_task(void *pvParameters) {
    while (1)
    {
        adc_cont_read();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }   
}