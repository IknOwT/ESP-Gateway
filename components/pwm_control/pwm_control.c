#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "pwm_control.h"

static const char *TAG = "PWM_CONTROL";

// PWM timer configuration
static ledc_timer_config_t pwm0_timer = {
    .speed_mode       = LEDC_LOW_SPEED_MODE,
    .timer_num        = LEDC_TIMER_0,
    .duty_resolution  = PWM_RESOLUTION,
    .freq_hz          = BZR_FREQUENCY,
    .clk_cfg          = LEDC_AUTO_CLK,
};
static ledc_timer_config_t pwm1_timer = {
    .speed_mode       = LEDC_LOW_SPEED_MODE,
    .timer_num        = LEDC_TIMER_1,
    .duty_resolution  = PWM_RESOLUTION,
    .freq_hz          = ALRT_FREQUENCY,
    .clk_cfg          = LEDC_AUTO_CLK,
};

// PWM channel configuration
static ledc_channel_config_t pwm0_channel = {
    .gpio_num   = BZR_GPIO,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel    = PWM_CHANNEL0,
    .intr_type  = LEDC_INTR_DISABLE,
    .timer_sel  = LEDC_TIMER_0,
    .duty       = 0,  // Initial duty cycle of 0
    .hpoint     = 0,
};
static ledc_channel_config_t pwm1_channel = {
    .gpio_num   = ALRT_GPIO,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel    = PWM_CHANNEL1,
    .intr_type  = LEDC_INTR_DISABLE,
    .timer_sel  = LEDC_TIMER_1,
    .duty       = 0,  // Initial duty cycle of 0
    .hpoint     = 0,
};

void pwm_init(void)
{
    // Initialize PWM timer
    ledc_timer_config(&pwm0_timer);
    ledc_timer_config(&pwm1_timer);

    // Initialize PWM channel
    ledc_channel_config(&pwm0_channel);
    ledc_channel_config(&pwm1_channel);

    ESP_LOGI(TAG, "PWM initialized succesfully");
}

static void buzzer_tone_sweep(int start_freq, int end_freq, int step, int delay_ms) {
    int freq = start_freq;
    while ((step > 0 && freq <= end_freq) || (step < 0 && freq >= end_freq)) {
        ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, freq);
        ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL0, BZR_DUTY);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL0);
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
        freq += step;
    }
}

void buzzer_boot_tone(void) {
    // Rising sweep: 1kHz → 4kHz
    buzzer_tone_sweep(1000, 4000, 200, 40);
    vTaskDelay(pdMS_TO_TICKS(100));
    // Falling sweep: 4kHz → 2kHz
    buzzer_tone_sweep(4000, 2000, -300, 30);
    ledc_stop(LEDC_LOW_SPEED_MODE, PWM_CHANNEL0, 0);
}

void buzzer_wifi_connected_tone(void) {
    // Chirp-like double sweep
    buzzer_tone_sweep(1500, 3000, 250, 20);
    vTaskDelay(pdMS_TO_TICKS(50));
    buzzer_tone_sweep(3000, 1500, -250, 20);
    ledc_stop(LEDC_LOW_SPEED_MODE, PWM_CHANNEL0, 0);
}

void buzzer_alert_tone(void *param) {
    TickType_t start_time = xTaskGetTickCount();
    TickType_t duration = pdMS_TO_TICKS(60000);  // 1 minute

    while ((xTaskGetTickCount() - start_time) < duration) {
        buzzer_tone_sweep(2000, 4000, 400, 15);
        buzzer_tone_sweep(4000, 2000, -400, 15);
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    ledc_stop(LEDC_LOW_SPEED_MODE, PWM_CHANNEL0, 0);
}

void led_alarm_pulse(void *param) {
    const TickType_t duration = pdMS_TO_TICKS(60000);  // 1 minute
    const TickType_t start_time = xTaskGetTickCount();

    while ((xTaskGetTickCount() - start_time) < duration) {
        // Fade in
        for (int duty = 0; duty <= 255; duty += 15) {
            ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL1, duty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL1);
            vTaskDelay(pdMS_TO_TICKS(10));
        }

        // Fade out
        for (int duty = 255; duty >= 0; duty -= 15) {
            ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL1, duty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL1);
            vTaskDelay(pdMS_TO_TICKS(10));
        }

        vTaskDelay(pdMS_TO_TICKS(100));  // Pause briefly between pulses
    }

    // Turn off LED after alarm ends
    ledc_stop(LEDC_LOW_SPEED_MODE, PWM_CHANNEL1, 0);
}