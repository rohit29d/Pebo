#include "pebo_board.h"
#include "driver/gpio.h"

static pebo_board_config_t s_board_config = {
    .primary_button_gpio = PEBO_PRIMARY_BUTTON_GPIO,
    .secondary_button_gpio = PEBO_SECONDARY_BUTTON_GPIO,
};

esp_err_t pebo_board_init(const pebo_board_config_t *config)
{
    if (config != NULL) {
        s_board_config = *config;
    }

    gpio_config_t io_config = {
        .pin_bit_mask = (1ULL << s_board_config.primary_button_gpio) |
                        (1ULL << s_board_config.secondary_button_gpio),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    return gpio_config(&io_config);
}

bool pebo_board_are_boot_buttons_pressed(void)
{
    int primary_level = gpio_get_level(s_board_config.primary_button_gpio);
    int secondary_level = gpio_get_level(s_board_config.secondary_button_gpio);

    return (primary_level == 0) && (secondary_level == 0);
}
