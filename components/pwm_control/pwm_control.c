#include "pwm_control.h"

static const char *TAG = "PWM_CONTROL";

// PWM timer configuration
static ledc_timer_config_t pwm_timer = {
    .speed_mode       = LEDC_LOW_SPEED_MODE,
    .timer_num        = LEDC_TIMER_0,
    .duty_resolution  = PWM_RESOLUTION,
    .freq_hz          = BZR_FREQUENCY,
    .clk_cfg          = LEDC_AUTO_CLK,
};

// PWM channel configuration
static ledc_channel_config_t pwm_channel = {
    .gpio_num   = BZR_GPIO,
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

static void buzzer_tone_sweep(int start_freq, int end_freq, int step, int delay_ms) {
    int freq = start_freq;
    while ((step > 0 && freq <= end_freq) || (step < 0 && freq >= end_freq)) {
        ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, freq);
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, BZR_DUTY);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
        freq += step;
    }
    ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
}

void buzzer_boot_tone(void) {
    // Rising sweep: 1kHz → 4kHz
    buzzer_tone_sweep(1000, 4000, 200, 40);
    vTaskDelay(pdMS_TO_TICKS(100));
    // Falling sweep: 4kHz → 2kHz
    buzzer_tone_sweep(4000, 2000, -300, 30);
    ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);

}

void buzzer_wifi_connected_tone(void) {
    // Chirp-like double sweep
    buzzer_tone_sweep(1500, 3000, 250, 20);
    vTaskDelay(pdMS_TO_TICKS(50));
    buzzer_tone_sweep(3000, 1500, -250, 20);
    ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
}

void buzzer_alert_tone(void) {
    // Fast pulsing up and down sweep
    for (int i = 0; i < 2; i++) {
        buzzer_tone_sweep(2000, 4000, 400, 15);
        buzzer_tone_sweep(4000, 2000, -400, 15);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
}