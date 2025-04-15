#include "i2c_control.h"
#include "esp_log.h"

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

esp_err_t scd41_power_down(void) {
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

esp_err_t scd41_wake_up(void) {
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

esp_err_t scd41_measure(void) {
    uint8_t cmd[2] = { (SCD4x_CMD_MEAS >> 8) & 0xFF, SCD4x_CMD_MEAS & 0xFF };

    esp_err_t ret = i2c_master_write_to_device(I2C_MASTER_NUM, I2C_SCD4x_ADDR, cmd, sizeof(cmd), pdMS_TO_TICKS(1000));

    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "SCD41 performing measurements...");
    } else {
        ESP_LOGE(TAG, "Failed to send measurement command to SCD41: %s", esp_err_to_name(ret));
    }

    return ret;
}

esp_err_t scd41_read(float *temperature, float *humidity) {
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
    *temperature = -45.0f + 175.0f * ((float)raw_temp / 65535.0f);
    *humidity = 100.0f * ((float)raw_hum / 65535.0f);

    ESP_LOGI(TAG, "CO2: %u ppm", co2);
    ESP_LOGI(TAG, "Temperature: %.2f°C", *temperature);
    ESP_LOGI(TAG, "Humidity: %.2f%%", *humidity);

    return ret;
}

// The task that reads the sensor periodically every 5 seconds
void scd41_task(void *arg) {
    while (1) {
        scd41_wake_up();
        vTaskDelay(pdMS_TO_TICKS(30)); // Sensor needs to wake up

        scd41_measure();  // Starts measurement
        vTaskDelay(pdMS_TO_TICKS(5000)); // Wait for measurement to be ready

        float temperature = 0.0f;
        float humidity = 0.0f;

        if (scd41_read(&temperature, &humidity)) {  // Assuming this returns bool and gives us values
            if (xSemaphoreTake(xSensorDataMutex, portMAX_DELAY)) {
                sensor_data.temperature = temperature;
                sensor_data.humidity = humidity;
                xSemaphoreGive(xSensorDataMutex);
            }

            ESP_LOGI(TAG, "Updated Temp: %.2f°C, Humidity: %.2f%%", temperature, humidity);
        } else {
            ESP_LOGW(TAG, "Failed to read from SCD41");
        }

        scd41_power_down();
        vTaskDelay(pdMS_TO_TICKS(9970)); // Total delay ≈ 15s
    }
}