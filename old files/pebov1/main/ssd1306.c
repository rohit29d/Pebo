#include "ssd1306.h"
#include "i2c_ctrl.h"
#include "driver/i2c.h"
#include <string.h>

#define OLED_ADDR 0x3C
#define WIDTH 128
#define HEIGHT 64

static uint8_t buffer[WIDTH * HEIGHT / 8];

static void cmd(uint8_t c)
{
    uint8_t data[2] = {0x00, c};
    i2c_master_write_to_device(I2C_PORT, OLED_ADDR, data, 2, 100);
}

void ssd1306_init(void)
{
    cmd(0xAE);
    cmd(0x20); cmd(0x00);
    cmd(0xB0);
    cmd(0xC8);
    cmd(0x02);
    cmd(0x10);
    cmd(0x40);
    cmd(0x81); cmd(0x7F);
    cmd(0xA1);
    cmd(0xA6);
    cmd(0xA8); cmd(0x3F);
    cmd(0xA4);
    cmd(0xD3); cmd(0x00);
    cmd(0xD5); cmd(0x80);
    cmd(0xD9); cmd(0xF1);
    cmd(0xDA); cmd(0x12);
    cmd(0xDB); cmd(0x40);
    cmd(0x8D); cmd(0x14);
    cmd(0xAF);
    ssd1306_clear();
}

void ssd1306_clear(void)
{
    memset(buffer, 0, sizeof(buffer));
    ssd1306_update();
}

void ssd1306_update(void)
{
    for (uint8_t page = 0; page < 8; page++) {
        cmd(0xB0 + page);
        cmd(0x00);
        cmd(0x10);
        uint8_t data[129];
        data[0] = 0x40;
        for (int i = 0; i < 128; i++) {
            data[i + 1] = buffer[i + page * 128];
        }
        i2c_master_write_to_device(I2C_PORT, OLED_ADDR, data, 129, 100);
    }
}

// THIS FUNCTION IS REQUIRED BY THE ANIMATION ENGINE
void ssd1306_draw_bitmap(int x, int y, const uint8_t *bitmap, int w, int h) {
    int16_t byteWidth = (w + 7) / 8;
    uint8_t byte = 0;

    for (int16_t j = 0; j < h; j++, y++) {
        for (int16_t i = 0; i < w; i++) {
            if (i & 7)
                byte <<= 1;
            else
                byte = bitmap[j * byteWidth + i / 8];
            
            if (byte & 0x80) {
                // Set pixel (white)
                if (x + i >= 0 && x + i < WIDTH && y >= 0 && y < HEIGHT) {
                    buffer[x + i + (y / 8) * WIDTH] |= (1 << (y & 7));
                }
            } else {
                // Clear pixel (black) - optional for transparency
                // if (x + i >= 0 && x + i < WIDTH && y >= 0 && y < HEIGHT) {
                //     buffer[x + i + (y / 8) * WIDTH] &= ~(1 << (y & 7));
                // }
            }
        }
    }
}