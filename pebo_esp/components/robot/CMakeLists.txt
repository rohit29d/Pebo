idf_component_register(SRCS "robot_system.c"
                       INCLUDE_DIRS "include"
                       REQUIRES board display_ssd1306 gyro motor settings esp_timer)
