#include "robot_system.h"
#include "display_ssd1306.h"
#include "esp_check.h"
#include "esp_log.h"
#include "gyro_adapter.h"
#include "motor_adapter.h"
#include "pebo_board.h"
#include "pebo_settings.h"

static const char *TAG = "robot_system";

static esp_err_t robot_draw_boot_frame(display_ssd1306_handle_t display)
{
    if (display == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    const uint16_t width = display_ssd1306_get_width(display);
    const uint16_t height = display_ssd1306_get_height(display);

    display_ssd1306_clear(display, false);
    display_ssd1306_draw_rect(display, 0, 0, width, height, true);
    display_ssd1306_draw_rect(display, 4, 4, width - 8, height - 8, true);
    display_ssd1306_fill_rect(display, width / 4, height / 3, width / 2, height / 3, true);

    return display_ssd1306_flush(display);
}

esp_err_t robot_system_start(void)
{
    ESP_RETURN_ON_ERROR(pebo_settings_storage_init(), TAG, "nvs init failed");
    ESP_RETURN_ON_ERROR(pebo_board_init(NULL), TAG, "board init failed");

    pebo_settings_t settings;
    if (pebo_board_are_boot_buttons_pressed()) {
        ESP_LOGI(TAG, "Boot buttons held, loading saved settings");
        pebo_settings_load(&settings);
    } else {
        pebo_settings_defaults(&settings);
        pebo_settings_save(&settings);
    }

    display_ssd1306_config_t display_config = DISPLAY_SSD1306_DEFAULT_CONFIG();
    display_ssd1306_handle_t display = NULL;
    esp_err_t display_err = display_ssd1306_init(&display_config, &display);
    if (display_err == ESP_OK) {
        ESP_RETURN_ON_ERROR(robot_draw_boot_frame(display), TAG, "display boot frame failed");
        ESP_LOGI(TAG, "Display adapter ready");
    } else {
        ESP_LOGW(TAG, "Display adapter unavailable: %s", esp_err_to_name(display_err));
    }

    esp_err_t gyro_err = gyro_adapter_init(NULL);
    if (gyro_err != ESP_OK) {
        ESP_LOGI(TAG, "Gyro adapter placeholder active");
    }

    esp_err_t motor_err = motor_adapter_init(NULL);
    if (motor_err != ESP_OK) {
        ESP_LOGI(TAG, "Motor adapter placeholder active");
    }

    ESP_LOGI(TAG, "Robot OS skeleton ready");
    return ESP_OK;
}
