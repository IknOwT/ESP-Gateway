#include "i2c_control.h"
#include "driver/i2c.h"
#include "esp_log.h"

#define I2C_MASTER_SCL_IO    22      // Set SCL GPIO
#define I2C_MASTER_SDA_IO    21      // Set SDA GPIO
#define I2C_MASTER_FREQ_HZ   100000  // 400kHz I2C frequency
#define I2C_MASTER_NUM       I2C_NUM_0  // I2C port number
#define I2C_MASTER_TIMEOUT   1000 / portTICK_PERIOD_MS  

#define I2C_SCD4x_ADDR       0x62 //SCD41 Address
#define SCD4x_CMD_POWER_DOWN 0x36e0  // Power Down Command
#define SCD4x_CMD_WAKE_UP    0x36f6  // Wake up Command
#define SCD4x_CMD_MEAS       0x219d  // Single shot measurement command
#define SCD4x_CMD_READ       0xeC05  //Read data from sensor

static const char *TAG = "i2c_control.c";

void i2c_init(void) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ
    };
    
    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0));

    ESP_LOGI(TAG, "I2C initialized successfully");
}

esp_err_t scd41_power_down() {
    uint8_t cmd[2] = { (SCD4x_CMD_POWER_DOWN >> 8) & 0xFF, SCD4x_CMD_POWER_DOWN & 0xFF };

    // Write the 2-byte command to the device
    esp_err_t ret = i2c_master_write_to_device(I2C_MASTER_NUM, I2C_SCD4x_ADDR, cmd, sizeof(cmd), pdMS_TO_TICKS(1000));

    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "SCD41 power down command sent successfully");
    } else {
        ESP_LOGE(TAG, "Failed to send power down command: %s", esp_err_to_name(ret));
    }

    return ret;
}

esp_err_t scd41_wake_up() {
    uint8_t cmd[2] = { (SCD4x_CMD_WAKE_UP >> 8) & 0xFF, SCD4x_CMD_WAKE_UP & 0xFF };

    // Write the 2-byte command to the device
    esp_err_t ret = i2c_master_write_to_device(I2C_MASTER_NUM, I2C_SCD4x_ADDR, cmd, sizeof(cmd), pdMS_TO_TICKS(1000));

    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "SCD41 wake up command sent successfully");
    } else {
        ESP_LOGI(TAG, "SCD41 wake up command sent successfully"); //since SCD41 does not acknowledge the wake-up command (0x36F6).
    }

    return ret;
}

esp_err_t scd41_measure() {
    uint8_t cmd[2] = { (SCD4x_CMD_MEAS >> 8) & 0xFF, SCD4x_CMD_MEAS & 0xFF };

    esp_err_t ret = i2c_master_write_to_device(I2C_MASTER_NUM, I2C_SCD4x_ADDR, cmd, sizeof(cmd), pdMS_TO_TICKS(1000));

    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "SCD41 performing measurements...");
    } else {
        ESP_LOGE(TAG, "Failed to send measurement command to SCD41: %s", esp_err_to_name(ret));
    }

    return ret;
}

esp_err_t scd41_read() {
    uint8_t cmd[2] = { (SCD4x_CMD_READ >> 8) & 0xFF, SCD4x_CMD_READ & 0xFF };

    esp_err_t ret = i2c_master_write_to_device(I2C_MASTER_NUM, I2C_SCD4x_ADDR, cmd, sizeof(cmd), pdMS_TO_TICKS(1000));

    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Reading sensor data...");
    } else {
        ESP_LOGE(TAG, "Failed to sensor data: %s", esp_err_to_name(ret));
    }

    uint8_t response[9] = {0};  // SCD41 sends 2 data bytes + 1 CRC byte

    ret = i2c_master_read_from_device(I2C_MASTER_NUM, I2C_SCD4x_ADDR, response, sizeof(response), pdMS_TO_TICKS(1000));
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read data from SCD41: %s", esp_err_to_name(ret));
        return ret;
    }

    uint16_t raw_co2 = ((uint16_t)response[0] << 8) | response[1];
    uint16_t raw_temp = ((uint16_t)response[3] << 8) | response[4];
    uint16_t raw_hum = ((uint16_t)response[6] << 8) | response[7];

    uint16_t co2 = raw_co2;  // CO2 is already in ppm
    float temperature = -45.0f + 175.0f * ((float)raw_temp / 65535.0f);
    float humidity = 100.0f * ((float)raw_hum / 65535.0f);

    ESP_LOGI(TAG, "CO2: %u ppm", co2);
    ESP_LOGI(TAG, "Temperature: %.2f°C", temperature);
    ESP_LOGI(TAG, "Humidity: %.2f%%", humidity);

    return ret;
}

// The task that reads the sensor periodically every 5 seconds
void scd41_task(void *arg) {
    while (1) {
        // Read from the sensor here
        scd41_wake_up();
        vTaskDelay(pdMS_TO_TICKS(30)); // Wait for 5 seconds
        scd41_measure();
        vTaskDelay(pdMS_TO_TICKS(5000)); // Wait for 5 seconds
        scd41_read();
        scd41_power_down();
        vTaskDelay(pdMS_TO_TICKS(9970)); // Wait for 5 seconds
    }
}