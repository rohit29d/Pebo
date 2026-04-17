#include <string.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "pebo_settings.h"

void pebo_settings_defaults(pebo_settings_t *settings)
{
    if (settings == NULL) {
        return;
    }

    memset(settings, 0, sizeof(*settings));
    settings->version = PEBO_SETTINGS_VERSION;
    settings->volume = 5;
    settings->sleeptime_minutes = 5;
}

esp_err_t pebo_settings_storage_init(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    return err;
}

esp_err_t pebo_settings_load(pebo_settings_t *settings)
{
    if (settings == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_handle_t handle;
    esp_err_t err = nvs_open("config", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        pebo_settings_defaults(settings);
        return err;
    }

    size_t required_size = sizeof(*settings);
    err = nvs_get_blob(handle, "pebo", settings, &required_size);
    nvs_close(handle);

    if (err != ESP_OK || required_size != sizeof(*settings) || settings->version != PEBO_SETTINGS_VERSION) {
        pebo_settings_defaults(settings);
        return (err == ESP_OK) ? ESP_ERR_INVALID_SIZE : err;
    }

    return ESP_OK;
}

esp_err_t pebo_settings_save(const pebo_settings_t *settings)
{
    if (settings == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_handle_t handle;
    esp_err_t err = nvs_open("config", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return err;
    }

    err = nvs_set_blob(handle, "pebo", settings, sizeof(*settings));
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }

    nvs_close(handle);
    return err;
}
