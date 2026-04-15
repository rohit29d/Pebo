#pragma once
#include "driver/i2c.h"

#define I2C_PORT I2C_NUM_0
#define I2C_SDA  8
#define I2C_SCL  9
#define I2C_FREQ 400000

void i2c_master_init(void);
