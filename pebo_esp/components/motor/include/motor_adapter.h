#pragma once

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int placeholder_pwm_channel;
} motor_adapter_config_t;

esp_err_t motor_adapter_init(const motor_adapter_config_t *config);
bool motor_adapter_is_ready(void);

#ifdef __cplusplus
}
#endif
