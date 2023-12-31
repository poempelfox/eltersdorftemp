
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/i2c.h"

void i2c_port_init(void)
{
    i2c_config_t i2cp0conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 19,  /* GPIO19 */
        .scl_io_num = 18,  /* GPIO18 */
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000, /* There is really no need to hurry */
    };
    i2c_param_config(I2C_NUM_0, &i2cp0conf);
    if (i2c_driver_install(I2C_NUM_0, i2cp0conf.mode, 0, 0, 0) != ESP_OK) {
        ESP_LOGI("edt-i2c.c", "Oh dear: I2C-Init for Port 0 failed.");
    } else {
        ESP_LOGI("edt-i2c.c", "I2C master port 0 initialized");
    }
    i2c_config_t i2cp1conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 32,  /* GPIO32 */
        .scl_io_num = 33,  /* GPIO33 */
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000, /* There is really no need to hurry */
    };
    i2c_param_config(I2C_NUM_1, &i2cp1conf);
    if (i2c_driver_install(I2C_NUM_1, i2cp1conf.mode, 0, 0, 0) != ESP_OK) {
        ESP_LOGI("edt-i2c.c", "Oh dear: I2C-Init for Port 1 failed.");
    } else {
        ESP_LOGI("edt-i2c.c", "I2C master port 1 initialized");
    }
}

