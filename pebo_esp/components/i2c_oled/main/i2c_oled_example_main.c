/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "esp_bit_defs.h"
#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_log.h"
#include "driver/i2c_master.h"

#if CONFIG_EXAMPLE_LCD_CONTROLLER_SH1107
#include "esp_lcd_sh1107.h"
#else
#include "esp_lcd_panel_vendor.h"
#endif

static const char *TAG = "i2c_oled";

#define I2C_BUS_PORT                  0
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ    (400 * 1000)
#define EXAMPLE_PIN_NUM_SDA           21
#define EXAMPLE_PIN_NUM_SCL           22
#define EXAMPLE_PIN_NUM_RST           -1
#define EXAMPLE_I2C_HW_ADDR           0x3C

#if CONFIG_EXAMPLE_LCD_CONTROLLER_SSD1306
#define EXAMPLE_LCD_H_RES             128
#define EXAMPLE_LCD_V_RES             CONFIG_EXAMPLE_SSD1306_HEIGHT
#elif CONFIG_EXAMPLE_LCD_CONTROLLER_SH1107
#define EXAMPLE_LCD_H_RES             64
#define EXAMPLE_LCD_V_RES             128
#endif

#define EXAMPLE_LCD_CMD_BITS          8
#define EXAMPLE_LCD_PARAM_BITS        8

static uint8_t oled_buffer[EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES / 8];

static const uint8_t font_5x7[][5] = {
    [' ' - ' '] = {0x00, 0x00, 0x00, 0x00, 0x00},
    ['B' - ' '] = {0x7F, 0x49, 0x49, 0x49, 0x36},
    ['C' - ' '] = {0x3E, 0x41, 0x41, 0x41, 0x22},
    ['D' - ' '] = {0x7F, 0x41, 0x41, 0x22, 0x1C},
    ['E' - ' '] = {0x7F, 0x49, 0x49, 0x49, 0x41},
    ['G' - ' '] = {0x3E, 0x41, 0x49, 0x49, 0x7A},
    ['I' - ' '] = {0x00, 0x41, 0x7F, 0x41, 0x00},
    ['L' - ' '] = {0x7F, 0x40, 0x40, 0x40, 0x40},
    ['N' - ' '] = {0x7F, 0x02, 0x04, 0x08, 0x7F},
    ['O' - ' '] = {0x3E, 0x41, 0x41, 0x41, 0x3E},
    ['P' - ' '] = {0x7F, 0x09, 0x09, 0x09, 0x06},
    ['R' - ' '] = {0x7F, 0x09, 0x19, 0x29, 0x46},
    ['T' - ' '] = {0x01, 0x01, 0x7F, 0x01, 0x01},
    ['V' - ' '] = {0x1F, 0x20, 0x40, 0x20, 0x1F},
};

static void oled_clear(void)
{
    memset(oled_buffer, 0, sizeof(oled_buffer));
}

static void oled_set_pixel(int x, int y, bool on)
{
    if (x < 0 || x >= EXAMPLE_LCD_H_RES || y < 0 || y >= EXAMPLE_LCD_V_RES) {
        return;
    }

    uint8_t *byte = &oled_buffer[EXAMPLE_LCD_H_RES * (y / 8) + x];
    if (on) {
        *byte |= BIT(y % 8);
    } else {
        *byte &= ~BIT(y % 8);
    }
}

static void oled_draw_char(int x, int y, char c)
{
    if (c < ' ' || c > 'V') {
        c = ' ';
    }

    const uint8_t *glyph = font_5x7[c - ' '];
    for (int col = 0; col < 5; col++) {
        for (int row = 0; row < 7; row++) {
            oled_set_pixel(x + col, y + row, glyph[col] & BIT(row));
        }
    }
}

static void oled_draw_text(int x, int y, const char *text)
{
    while (*text) {
        oled_draw_char(x, y, *text++);
        x += 6;
    }
}

static void oled_draw_frame(void)
{
    for (int x = 0; x < EXAMPLE_LCD_H_RES; x++) {
        oled_set_pixel(x, 0, true);
        oled_set_pixel(x, EXAMPLE_LCD_V_RES - 1, true);
    }
    for (int y = 0; y < EXAMPLE_LCD_V_RES; y++) {
        oled_set_pixel(0, y, true);
        oled_set_pixel(EXAMPLE_LCD_H_RES - 1, y, true);
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "Initialize I2C bus");
    i2c_master_bus_handle_t i2c_bus = NULL;
    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .i2c_port = I2C_BUS_PORT,
        .sda_io_num = EXAMPLE_PIN_NUM_SDA,
        .scl_io_num = EXAMPLE_PIN_NUM_SCL,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &i2c_bus));

    ESP_LOGI(TAG, "Install panel IO");
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = EXAMPLE_I2C_HW_ADDR,
        .scl_speed_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
        .control_phase_bytes = 1,
        .lcd_cmd_bits = EXAMPLE_LCD_CMD_BITS,
        .lcd_param_bits = EXAMPLE_LCD_PARAM_BITS,
#if CONFIG_EXAMPLE_LCD_CONTROLLER_SSD1306
        .dc_bit_offset = 6,
#elif CONFIG_EXAMPLE_LCD_CONTROLLER_SH1107
        .dc_bit_offset = 0,
        .flags = {
            .disable_control_phase = 1,
        },
#endif
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(i2c_bus, &io_config, &io_handle));

    ESP_LOGI(TAG, "Install OLED panel driver");
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .bits_per_pixel = 1,
        .reset_gpio_num = EXAMPLE_PIN_NUM_RST,
    };
#if CONFIG_EXAMPLE_LCD_CONTROLLER_SSD1306
    esp_lcd_panel_ssd1306_config_t ssd1306_config = {
        .height = EXAMPLE_LCD_V_RES,
    };
    panel_config.vendor_config = &ssd1306_config;
    ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));
#elif CONFIG_EXAMPLE_LCD_CONTROLLER_SH1107
    ESP_ERROR_CHECK(esp_lcd_new_panel_sh1107(io_handle, &panel_config, &panel_handle));
#endif

    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

#if CONFIG_EXAMPLE_LCD_CONTROLLER_SH1107
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true));
#endif

    ESP_LOGI(TAG, "Draw directly to OLED");
    oled_clear();
    oled_draw_frame();
    oled_draw_text(8, 16, "PEBO OLED");
    oled_draw_text(8, 32, "DIRECT IO");
    ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, EXAMPLE_LCD_H_RES, EXAMPLE_LCD_V_RES, oled_buffer));
}
