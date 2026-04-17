#include "motor_adapter.h"

static bool s_motor_ready = false;

esp_err_t motor_adapter_init(const motor_adapter_config_t *config)
{
    (void)config;
    s_motor_ready = false;
    return ESP_ERR_NOT_SUPPORTED;
}

bool motor_adapter_is_ready(void)
{
    return s_motor_ready;
}
