idf_component_register(SRCS "adc_control.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_adc sensor_data)