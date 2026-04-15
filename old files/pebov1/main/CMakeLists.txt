idf_component_register(
    SRCS
        "main.c"
        "touch_ctrl.c"
        "state_machine.c"
        "ssd1306.c"
        "pebo_eyes.c"
        "i2c_ctrl.c"            # <--- ADDED THIS LINE
        "Irisoled.cpp"          # Make sure this file exists in main/
        "IrisoledAnimation.cpp" # Make sure this file exists in main/
        "irisoled_c_api.cpp"
    INCLUDE_DIRS
        "include"
        "." # Add this so it finds headers in the main folder if they are there
)