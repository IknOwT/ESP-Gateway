#ifndef PWM_CONTROL_H
#define PWM_CONTROL_H

#include "driver/ledc.h"

// PWM configuration parameters
#define PWM_CHANNEL    LEDC_CHANNEL_0
#define PWM_GPIO_PIN   17  // GPIO pin to control the PWM
#define PWM_FREQUENCY  4000  // Frequency of PWM (in Hz)
#define PWM_RESOLUTION LEDC_TIMER_13_BIT  // 13-bit resolution (0 to 8191)

void pwm_init(void);
void pwm_set_duty(uint32_t duty);
void pwm_start(void);
void pwm_stop(void);

#endif // PWM_CONTROL_H