#include "esp_err.h"
#include "esp_log.h"
#include "robot_system.h"

static const char *TAG = "pebo_main";

void app_main(void)
{
    esp_err_t err = robot_system_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Robot system failed to start: %s", esp_err_to_name(err));
    }
}
