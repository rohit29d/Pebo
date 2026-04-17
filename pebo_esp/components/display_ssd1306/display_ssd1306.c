#include <stdlib.h>
#include <string.h>
#include "display_ssd1306.h"
#include "esp_check.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"

#define SSD1306_LCD_CMD_BITS    8
#define SSD1306_LCD_PARAM_BITS  8

static const char *TAG = "display_ssd1306";

struct display_ssd1306_t {
    i2c_master_bus_handle_t i2c_bus;
    esp_lcd_panel_io_handle_t io_handle;
    esp_lcd_panel_handle_t panel_handle;
    uint8_t *buffer;
    size_t buffer_size;
    uint16_t width;
    uint16_t height;
};

static bool display_ssd1306_is_valid(display_ssd1306_handle_t handle)
{
    return handle != NULL && handle->buffer != NULL && handle->panel_handle != NULL;
}

static void display_ssd1306_set_pixel(display_ssd1306_handle_t handle, int x, int y, bool color_on)
{
    if (!display_ssd1306_is_valid(handle)) {
        return;
    }
    if (x < 0 || x >= handle->width || y < 0 || y >= handle->height) {
        return;
    }

    uint16_t index = x + (y / 8) * handle->width;
    uint8_t mask = (uint8_t)(1U << (y % 8));
    if (color_on) {
        handle->buffer[index] |= mask;
    } else {
        handle->buffer[index] &= (uint8_t)~mask;
    }
}

static void display_ssd1306_draw_hline(display_ssd1306_handle_t handle, int x, int y, int width, bool color_on)
{
    for (int i = 0; i < width; i++) {
        display_ssd1306_set_pixel(handle, x + i, y, color_on);
    }
}

esp_err_t display_ssd1306_init(const display_ssd1306_config_t *config, display_ssd1306_handle_t *out_handle)
{
    ESP_RETURN_ON_FALSE(config != NULL, ESP_ERR_INVALID_ARG, TAG, "config is null");
    ESP_RETURN_ON_FALSE(out_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "out_handle is null");

    esp_err_t ret = ESP_OK;
    display_ssd1306_handle_t handle = calloc(1, sizeof(struct display_ssd1306_t));
    ESP_RETURN_ON_FALSE(handle != NULL, ESP_ERR_NO_MEM, TAG, "no memory for display handle");

    handle->width = config->width;
    handle->height = config->height;
    handle->buffer_size = ((size_t)config->width * config->height) / 8;
    handle->buffer = calloc(1, handle->buffer_size);
    if (handle->buffer == NULL) {
        free(handle);
        return ESP_ERR_NO_MEM;
    }

    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .i2c_port = config->i2c_port,
        .sda_io_num = config->sda_io_num,
        .scl_io_num = config->scl_io_num,
        .flags.enable_internal_pullup = config->enable_internal_pullup,
    };
    ESP_GOTO_ON_ERROR(i2c_new_master_bus(&bus_config, &handle->i2c_bus), err, TAG, "create i2c bus failed");

    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = config->i2c_addr,
        .scl_speed_hz = config->pixel_clock_hz,
        .control_phase_bytes = 1,
        .lcd_cmd_bits = SSD1306_LCD_CMD_BITS,
        .lcd_param_bits = SSD1306_LCD_PARAM_BITS,
        .dc_bit_offset = 6,
    };
    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_io_i2c(handle->i2c_bus, &io_config, &handle->io_handle), err, TAG, "create panel io failed");

    esp_lcd_panel_dev_config_t panel_config = {
        .bits_per_pixel = 1,
        .reset_gpio_num = config->reset_io_num,
    };
    esp_lcd_panel_ssd1306_config_t ssd1306_config = {
        .height = config->height,
    };
    panel_config.vendor_config = &ssd1306_config;
    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_ssd1306(handle->io_handle, &panel_config, &handle->panel_handle), err, TAG, "create panel failed");

    ESP_GOTO_ON_ERROR(esp_lcd_panel_reset(handle->panel_handle), err, TAG, "panel reset failed");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_init(handle->panel_handle), err, TAG, "panel init failed");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_disp_on_off(handle->panel_handle, true), err, TAG, "display on failed");

    *out_handle = handle;
    return ESP_OK;

err:
    display_ssd1306_deinit(handle);
    return ret;
}

esp_err_t display_ssd1306_deinit(display_ssd1306_handle_t handle)
{
    if (handle == NULL) {
        return ESP_OK;
    }

    if (handle->panel_handle != NULL) {
        esp_lcd_panel_disp_on_off(handle->panel_handle, false);
        esp_lcd_panel_del(handle->panel_handle);
    }
    if (handle->io_handle != NULL) {
        esp_lcd_panel_io_del(handle->io_handle);
    }
    if (handle->i2c_bus != NULL) {
        i2c_del_master_bus(handle->i2c_bus);
    }

    free(handle->buffer);
    free(handle);
    return ESP_OK;
}

void display_ssd1306_clear(display_ssd1306_handle_t handle, bool color_on)
{
    if (!display_ssd1306_is_valid(handle)) {
        return;
    }
    memset(handle->buffer, color_on ? 0xFF : 0x00, handle->buffer_size);
}

void display_ssd1306_draw_rect(display_ssd1306_handle_t handle, int x, int y, int width, int height, bool color_on)
{
    if (width <= 0 || height <= 0) {
        return;
    }

    display_ssd1306_draw_hline(handle, x, y, width, color_on);
    display_ssd1306_draw_hline(handle, x, y + height - 1, width, color_on);

    for (int row = 0; row < height; row++) {
        display_ssd1306_set_pixel(handle, x, y + row, color_on);
        display_ssd1306_set_pixel(handle, x + width - 1, y + row, color_on);
    }
}

void display_ssd1306_fill_rect(display_ssd1306_handle_t handle, int x, int y, int width, int height, bool color_on)
{
    if (width <= 0 || height <= 0) {
        return;
    }

    for (int row = 0; row < height; row++) {
        display_ssd1306_draw_hline(handle, x, y + row, width, color_on);
    }
}

esp_err_t display_ssd1306_flush(display_ssd1306_handle_t handle)
{
    ESP_RETURN_ON_FALSE(display_ssd1306_is_valid(handle), ESP_ERR_INVALID_STATE, TAG, "display not initialized");
    return esp_lcd_panel_draw_bitmap(handle->panel_handle, 0, 0, handle->width, handle->height, handle->buffer);
}

uint16_t display_ssd1306_get_width(display_ssd1306_handle_t handle)
{
    return handle == NULL ? 0 : handle->width;
}

uint16_t display_ssd1306_get_height(display_ssd1306_handle_t handle)
{
    return handle == NULL ? 0 : handle->height;
}
