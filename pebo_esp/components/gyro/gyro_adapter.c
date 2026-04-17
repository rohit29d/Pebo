#include "gyro_adapter.h"

static bool s_gyro_ready = false;

esp_err_t gyro_adapter_init(const gyro_adapter_config_t *config)
{
    (void)config;
    s_gyro_ready = false;
    return ESP_ERR_NOT_SUPPORTED;
}

bool gyro_adapter_is_ready(void)
{
    return s_gyro_ready;
}






