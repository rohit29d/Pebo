#pragma once

#include <stdbool.h>
#include "driver/gpio.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PEBO_PRIMARY_BUTTON_GPIO GPIO_NUM_1
#define PEBO_SECONDARY_BUTTON_GPIO GPIO_NUM_2

typedef struct {
    gpio_num_t primary_button_gpio;
    gpio_num_t secondary_button_gpio;
} pebo_board_config_t;

esp_err_t pebo_board_init(const pebo_board_config_t *config);
bool pebo_board_are_boot_buttons_pressed(void);

#ifdef __cplusplus
}
#endif
