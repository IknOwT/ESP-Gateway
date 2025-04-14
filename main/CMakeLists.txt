idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES gpio_control pwm_control i2c_control adc_control)
