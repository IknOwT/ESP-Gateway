#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pwm_control.h"
#include "i2c_control.h"
#include "adc_control.h"

void app_main(void)
{
    i2c_init();     //  Initialize I2C
    pwm_init();     //  Initialize the PWM
    adc_init();     //  Initialize ADC

    xTaskCreate(scd41_task, "scd41_task", 2048, NULL, 5, NULL);
    xTaskCreate(mems_task, "mems_task", 2048, NULL, 5, NULL);
}
