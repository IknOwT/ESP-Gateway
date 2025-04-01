idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES pwm_control i2c_control adc_control)
