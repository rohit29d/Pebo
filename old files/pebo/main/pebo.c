#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_err.h"

/* ================= I2C CONFIG ================= */

#define I2C_MASTER_NUM   I2C_NUM_0
#define I2C_SDA          8
#define I2C_SCL          9
#define I2C_FREQ_HZ      400000

/* ================= OLED CONFIG ================= */

#define OLED_ADDR   0x3C
#define OLED_WIDTH  128
#define OLED_HEIGHT 64
#define OLED_PAGES  (OLED_HEIGHT / 8)

/* ================= FRAMEBUFFER ================= */

static uint8_t fb[OLED_WIDTH * OLED_PAGES];

/* ================= FUNCTION PROTOTYPES ================= */

static void oled_cmd(uint8_t cmd);

void fb_clear(void);
void fb_pixel(uint8_t x, uint8_t y, uint8_t on);
void fb_flush(void);

void oled_draw_char(uint8_t x, uint8_t y, char c);
void oled_draw_string(uint8_t x, uint8_t y, const char *str);
void oled_text_test(void);

/* ================= I2C INIT ================= */

void i2c_init(void)
{
    i2c_config_t cfg = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA,
        .scl_io_num = I2C_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_FREQ_HZ
    };

    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &cfg));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, cfg.mode, 0, 0, 0));
}

/* ================= LOW LEVEL OLED ================= */

static void oled_cmd(uint8_t cmd)
{
    i2c_cmd_handle_t h = i2c_cmd_link_create();
    i2c_master_start(h);
    i2c_master_write_byte(h, (OLED_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(h, 0x00, true);   // Command mode
    i2c_master_write_byte(h, cmd, true);
    i2c_master_stop(h);
    i2c_master_cmd_begin(I2C_MASTER_NUM, h, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(h);
}

/* ================= OLED INIT ================= */

void oled_init(void)
{
    oled_cmd(0xAE);                // Display OFF
    oled_cmd(0xD5); oled_cmd(0x80);
    oled_cmd(0xA8); oled_cmd(0x3F); // 64 rows
    oled_cmd(0xD3); oled_cmd(0x00);
    oled_cmd(0x40);
    oled_cmd(0x8D); oled_cmd(0x14);
    oled_cmd(0x20); oled_cmd(0x00); // Horizontal addressing
    oled_cmd(0xA1);
    oled_cmd(0xC8);
    oled_cmd(0xDA); oled_cmd(0x12);
    oled_cmd(0x81); oled_cmd(0x7F);
    oled_cmd(0xD9); oled_cmd(0xF1);
    oled_cmd(0xDB); oled_cmd(0x40);
    oled_cmd(0xA4);
    oled_cmd(0xA6);
    oled_cmd(0xAF);                // Display ON
}

/* ================= FRAMEBUFFER OPS ================= */

void fb_clear(void)
{
    for (int i = 0; i < (int)sizeof(fb); i++)
        fb[i] = 0x00;
}

void fb_pixel(uint8_t x, uint8_t y, uint8_t on)
{
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT) return;

    uint16_t idx = x + (y / 8) * OLED_WIDTH;
    uint8_t bit  = 1 << (y % 8);

    if (on) fb[idx] |= bit;
    else    fb[idx] &= ~bit;
}

void fb_flush(void)
{
    for (uint8_t page = 0; page < OLED_PAGES; page++) {
        oled_cmd(0xB0 + page);
        oled_cmd(0x00);
        oled_cmd(0x10);

        i2c_cmd_handle_t h = i2c_cmd_link_create();
        i2c_master_start(h);
        i2c_master_write_byte(h, (OLED_ADDR << 1) | I2C_MASTER_WRITE, true);
        i2c_master_write_byte(h, 0x40, true); // Data mode

        for (int col = 0; col < OLED_WIDTH; col++) {
            i2c_master_write_byte(h, fb[page * OLED_WIDTH + col], true);
        }

        i2c_master_stop(h);
        i2c_master_cmd_begin(I2C_MASTER_NUM, h, pdMS_TO_TICKS(100));
        i2c_cmd_link_delete(h);
    }
}

/* ================= FONT (5x7) ================= */
static const uint8_t font5x7[][5] = {
/* SPACE */ {0x00,0x00,0x00,0x00,0x00},
/* 0 */ {0x3E,0x51,0x49,0x45,0x3E},
/* 1 */ {0x00,0x42,0x7F,0x40,0x00},
/* 2 */ {0x42,0x61,0x51,0x49,0x46},
/* 3 */ {0x21,0x41,0x45,0x4B,0x31},
/* 4 */ {0x18,0x14,0x12,0x7F,0x10},
/* 5 */ {0x27,0x45,0x45,0x45,0x39},
/* 6 */ {0x3C,0x4A,0x49,0x49,0x30},
/* 7 */ {0x01,0x71,0x09,0x05,0x03},
/* 8 */ {0x36,0x49,0x49,0x49,0x36},
/* 9 */ {0x06,0x49,0x49,0x29,0x1E},

/* A */ {0x7E,0x09,0x09,0x09,0x7E},
/* B */ {0x7F,0x49,0x49,0x49,0x36},
/* C */ {0x3E,0x41,0x41,0x41,0x22},
/* D */ {0x7F,0x41,0x41,0x22,0x1C},
/* E */ {0x7F,0x49,0x49,0x49,0x41},
/* F */ {0x7F,0x09,0x09,0x09,0x01},
/* G */ {0x3E,0x41,0x49,0x49,0x7A},
/* H */ {0x7F,0x08,0x08,0x08,0x7F},
/* I */ {0x00,0x41,0x7F,0x41,0x00},
/* J */ {0x20,0x40,0x41,0x3F,0x01},
/* K */ {0x7F,0x08,0x14,0x22,0x41},
/* L */ {0x7F,0x40,0x40,0x40,0x40},
/* M */ {0x7F,0x02,0x0C,0x02,0x7F},
/* N */ {0x7F,0x04,0x08,0x10,0x7F},
/* O */ {0x3E,0x41,0x41,0x41,0x3E},
/* P */ {0x7F,0x09,0x09,0x09,0x06},
/* Q */ {0x3E,0x41,0x51,0x21,0x5E},
/* R */ {0x7F,0x09,0x19,0x29,0x46},
/* S */ {0x46,0x49,0x49,0x49,0x31},
/* T */ {0x01,0x01,0x7F,0x01,0x01},
/* U */ {0x3F,0x40,0x40,0x40,0x3F},
/* V */ {0x1F,0x20,0x40,0x20,0x1F},
/* W */ {0x3F,0x40,0x38,0x40,0x3F},
/* X */ {0x63,0x14,0x08,0x14,0x63},
/* Y */ {0x07,0x08,0x70,0x08,0x07},
/* Z */ {0x61,0x51,0x49,0x45,0x43},
};

/* ================= TEXT RENDERING ================= */
void oled_draw_char(uint8_t x, uint8_t y, char c)
{
    int index = -1;

    if (c == ' ') index = 0;
    else if (c >= '0' && c <= '9') index = 1 + (c - '0');
    else if (c >= 'A' && c <= 'Z') index = 11 + (c - 'A');
    else return;  // unsupported

    for (uint8_t col = 0; col < 5; col++) {
        uint8_t line = font5x7[index][col];
        for (uint8_t row = 0; row < 7; row++) {
            fb_pixel(
                x + col,
                y + row,
                (line >> row) & 0x01
            );
        }
    }
}


void oled_draw_string(uint8_t x, uint8_t y, const char *str)
{
    while (*str) {
        oled_draw_char(x, y, *str++);
        x += 6;
        if (x + 5 >= OLED_WIDTH) break;
    }
}

void oled_text_test(void)
{
    fb_clear();

    oled_draw_string(0,  0, "ESP32-C3");
    oled_draw_string(0, 10, "OLED OK");
    oled_draw_string(0, 20, "TEXT MODE");

    fb_flush();
}

/* ================= MAIN ================= */

void app_main(void)
{
    i2c_init();
    vTaskDelay(pdMS_TO_TICKS(100));

    oled_init();
    oled_text_test();
}
