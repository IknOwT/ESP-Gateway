#ifndef ADC_CONTROL_H
#define ADC_CONTROL_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_system.h"

#define ADC_WIDTH     ADC_WIDTH_BIT_12  // ADC resolution: 12-bit (4095)
#define ADC_CHANNEL   ADC1_CHANNEL_4   // ADC1 Channel 0 (GPIO33)
#define ADC_ATTEN     ADC_ATTEN_DB_12    // Input voltage range: 0-3.3V

void adc_init(void);
void mems_task(void *arg);

#endif

