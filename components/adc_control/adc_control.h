#ifndef ADC_CONTROL_H
#define ADC_CONTROL_H

#include "esp_adc/adc_continuous.h"
#include "sensor_data.h"

#define ADC_BUF_SIZE         2048      // fits 1 frame of adc data
#define CONV_FRAME_SIZE      2048      // equivalent to 1024 samples per frame as there are 2bytes per sample in 12bit adc.
#define ADC_SAMPLING         20000     // 20000 samples per second
#define ADC_MAX_VALUE        4095.0    // 12-bit ADC
#define ADC_REF_VOLTAGE      3.9       // Reference voltage in volts

void cont_adc_init(void);
void mems_task(void *pvParameters);

#endif

