# This CMakeLists.txt is in the pwm_control component folder

idf_component_register(SRCS "pwm_control.c"
                       INCLUDE_DIRS "."
                       REQUIRES driver)