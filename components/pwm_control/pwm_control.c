#include "pwm_control.h"

// PWM timer configuration
static ledc_timer_config_t pwm_timer = {
    .speed_mode       = LEDC_LOW_SPEED_MODE,
    .timer_num        = LEDC_TIMER_0,
    .duty_resolution  = PWM_RESOLUTION,
    .freq_hz          = PWM_FREQUENCY,
    .clk_cfg          = LEDC_AUTO_CLK,
};

// PWM channel configuration
static ledc_channel_config_t pwm_channel = {
    .gpio_num   = PWM_GPIO_PIN,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel    = PWM_CHANNEL,
    .intr_type  = LEDC_INTR_DISABLE,
    .timer_sel  = LEDC_TIMER_0,
    .duty       = 0,  // Initial duty cycle of 0
    .hpoint     = 0,
};

void pwm_init(void)
{
    // Initialize PWM timer
    ledc_timer_config(&pwm_timer);
    
    // Initialize PWM channel
    ledc_channel_config(&pwm_channel);
}

void pwm_set_duty(uint32_t duty)
{
    // Set the duty cycle (0 to 8191 for 13-bit resolution)
    ledc_set_duty(pwm_channel.speed_mode, pwm_channel.channel, duty);
    ledc_update_duty(pwm_channel.speed_mode, pwm_channel.channel);
}

void pwm_start(void)
{
    // Start the PWM signal generation
    ledc_update_duty(pwm_channel.speed_mode, pwm_channel.channel);
}

void pwm_stop(void)
{
    // Stop the PWM signal generation
    ledc_stop(pwm_channel.speed_mode, pwm_channel.channel, 0);
}