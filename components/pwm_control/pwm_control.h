#ifndef PWM_CONTROL_H
#define PWM_CONTROL_H

#include "driver/ledc.h"

// PWM configuration parameters
#define PWM_CHANNEL0         LEDC_CHANNEL_0
#define PWM_CHANNEL1         LEDC_CHANNEL_1
#define BZR_GPIO             17  // GPIO pin to control the PWM
#define ALRT_GPIO            18  // GPIO pin to control the PWM
#define BZR_FREQUENCY        4000  // Frequency of PWM (in Hz)
#define ALRT_FREQUENCY       4000  // Frequency of PWM (in Hz)
#define PWM_RESOLUTION       LEDC_TIMER_13_BIT  // 13-bit resolution (0 to 8191)
#define BZR_DUTY             4095
#define ALRT_DUTY            4095

void pwm_init(void);
void buzzer_boot_tone(void);
void buzzer_wifi_connected_tone(void);
void buzzer_alert_tone(void *param);
void led_alarm_pulse(void *param);

#endif // PWM_CONTROL_H