| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- |

# I2C OLED example

[esp_lcd](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/lcd/i2c_lcd.html) supports I2C interfaced OLED LCD, whose color depth is usually 1bpp.

This example shows how to make use of the SSD1306 panel driver from the `esp_lcd` component. It draws directly to a 1-bit framebuffer and sends that buffer to the OLED over I2C.

This folder is self-contained. It was copied from the ESP-IDF examples tree, but it can be built directly from this directory without keeping the original examples directory around.

## How to use the example

### Hardware Required

* An ESP development board
* An SSD1306 OLED LCD, with I2C interface
* An USB cable for power supply and programming

### Hardware Connection

The connection between ESP Board and the LCD is as follows:

```text
      ESP Board                       OLED LCD (I2C)
+------------------+              +-------------------+
|               GND+--------------+GND                |
|                  |              |                   |
|               3V3+--------------+VCC                |
|                  |              |                   |
|               SDA+--------------+SDA                |
|                  |              |                   |
|               SCL+--------------+SCL                |
+------------------+              +-------------------+
```

The GPIO number used by this example can be changed in [i2c_oled_example_main.c](main/i2c_oled_example_main.c). Please pay attention to the I2C hardware device address as well, you should refer to your module's spec and schematic to determine that address.

Default ESP32 wiring in this copy:

```text
SDA -> GPIO21
SCL -> GPIO22
```

### Build and Flash

From this directory, run `idf.py -p PORT build flash monitor` to build, flash and monitor the project:

```powershell
cd components\i2c_oled
idf.py -p COM_PORT build flash monitor
```

A scrolling text will show up on the LCD as expected.

The first time you run `idf.py` for the example may take extra time as the build system resolves component dependencies and downloads missing components from the ESP Component Registry into the `managed_components` folder.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

### Example Output

```bash
...
I (308) main_task: Started on CPU0
I (318) main_task: Calling app_main()
I (318) example: Initialize I2C bus
I (318) gpio: GPIO[3]| InputEn: 1| OutputEn: 1| OpenDrain: 1| Pullup: 1| Pulldown: 0| Intr:0
I (328) gpio: GPIO[4]| InputEn: 1| OutputEn: 1| OpenDrain: 1| Pullup: 1| Pulldown: 0| Intr:0
I (338) example: Install panel IO
I (338) example: Install SSD1306 panel driver
I (448) i2c_oled: Draw directly to OLED
I (448) main_task: Returned from app_main()
...
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
