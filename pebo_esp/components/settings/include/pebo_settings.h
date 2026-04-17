#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PEBO_SETTINGS_VERSION 1

typedef struct {
    uint8_t version;
    bool bluetooth;
    int volume;
    int sleeptime_minutes;
    bool exit_requested;
    bool no_move_mode;
    bool mute;
} pebo_settings_t;

void pebo_settings_defaults(pebo_settings_t *settings);
esp_err_t pebo_settings_storage_init(void);
esp_err_t pebo_settings_load(pebo_settings_t *settings);
esp_err_t pebo_settings_save(const pebo_settings_t *settings);

#ifdef __cplusplus
}
#endif
