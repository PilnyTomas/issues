#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#define I2C_MASTER_NUM 0
static const char *TAG = "i2c-simple-example";

/*
bool stay(){
  delay(5);
  if (Serial.available() > 0) {
    Serial.read();
    ESP_LOGI(TAG, "Recevied byte - continuing....");
    return false;
  }
  return true;
}
*/
esp_err_t init(uint32_t freq){

  i2c_config_t conf = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = 23,         // select GPIO specific to your project
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_io_num = 22,         // select GPIO specific to your project
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master.clk_speed = freq,  // select frequency specific to your project
    //.master = the_master  // select frequency specific to your project
    // .clk_flags = 0,          /*!< Optional, you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here. */
  };
  i2c_param_config(I2C_MASTER_NUM, &conf);
  return i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, I2C_MASTER_NUM);
}


void app_main() {
  uint32_t frequency;
  const uint8_t the_answer = 0x42;
  /* Install UART driver for interrupt-driven reads and writes */
  //ESP_ERROR_CHECK( uart_driver_install(CONFIG_CONSOLE_UART_NUM, 256, 0, 0, NULL, 0) );
  //esp_vfs_dev_uart_use_nonblocking

  /* Tell VFS to use UART driver */
  //esp_vfs_dev_uart_use_driver(CONFIG_CONSOLE_UART_NUM);

  ESP_LOGI(TAG, "TEST READY - send single byte to start");
  //while(stay());

  frequency = 250000; // According to user this on the border of accurate clock setting
  init(frequency);
  ESP_LOGI(TAG, "frequency=%u\n", frequency);
  i2c_master_write_to_device(I2C_MASTER_NUM, the_answer, &the_answer, 1, 10);
  ESP_ERROR_CHECK(i2c_driver_delete(I2C_MASTER_NUM));

  frequency = 750000;
  init(frequency);
  ESP_LOGI(TAG, "frequency=%u\n", frequency);
  i2c_master_write_to_device(I2C_MASTER_NUM, the_answer, &the_answer, 1, 10);
  ESP_ERROR_CHECK(i2c_driver_delete(I2C_MASTER_NUM));

  ESP_LOGI(TAG, "END OF TEST");
}

void loop() {
}
