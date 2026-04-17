#pragma once

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int placeholder_bus;
} gyro_adapter_config_t;

esp_err_t gyro_adapter_init(const gyro_adapter_config_t *config);
bool gyro_adapter_is_ready(void);

#ifdef __cplusplus
}
#endif
