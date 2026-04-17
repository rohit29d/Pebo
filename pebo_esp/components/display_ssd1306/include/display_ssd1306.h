#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct display_ssd1306_t *display_ssd1306_handle_t;

typedef struct {
    i2c_port_num_t i2c_port;
    gpio_num_t sda_io_num;
    gpio_num_t scl_io_num;
    gpio_num_t reset_io_num;
    uint8_t i2c_addr;
    uint32_t pixel_clock_hz;
    uint16_t width;
    uint16_t height;
    bool enable_internal_pullup;
} display_ssd1306_config_t;

#define DISPLAY_SSD1306_DEFAULT_CONFIG() { \
    .i2c_port = I2C_NUM_0, \
    .sda_io_num = GPIO_NUM_21, \
    .scl_io_num = GPIO_NUM_22, \
    .reset_io_num = GPIO_NUM_NC, \
    .i2c_addr = 0x3C, \
    .pixel_clock_hz = 400000, \
    .width = 128, \
    .height = 64, \
    .enable_internal_pullup = true, \
}

esp_err_t display_ssd1306_init(const display_ssd1306_config_t *config, display_ssd1306_handle_t *out_handle);
esp_err_t display_ssd1306_deinit(display_ssd1306_handle_t handle);
void display_ssd1306_clear(display_ssd1306_handle_t handle, bool color_on);
void display_ssd1306_draw_rect(display_ssd1306_handle_t handle, int x, int y, int width, int height, bool color_on);
void display_ssd1306_fill_rect(display_ssd1306_handle_t handle, int x, int y, int width, int height, bool color_on);
esp_err_t display_ssd1306_flush(display_ssd1306_handle_t handle);
uint16_t display_ssd1306_get_width(display_ssd1306_handle_t handle);
uint16_t display_ssd1306_get_height(display_ssd1306_handle_t handle);

#ifdef __cplusplus
}
#endif
