// https://github.com/espressif/arduino-esp32/issues/5938
//The audio output using I2S works both with an external DAC or in PDM mode but not using the internal DAC.
//This seems to be related to the ESP-IDF https://github.com/espressif/esp-idf/issues/7684

#include <Arduino.h>
#include "driver/i2s.h"

// driver settings
uint32_t                sample_rate = 16000;     /*!< I2S sample rate */
i2s_bits_per_sample_t   bits_per_sample = (i2s_bits_per_sample_t)16;    /*!< I2S sample bits in one channel */

//square wave generator settings
const int frequency = 1000; // frequency of square wave in Hz
const int16_t amplitude = 15000; // amplitude of square wave
const int halfWavelength = (sample_rate / frequency); // half wavelength of square wave
int16_t sample[2] = {amplitude, amplitude}; // current sample value
int count = 0;

void halt(){
  log_i("Halted.");
  while(1) vTaskDelay(100);
}

void setup() {
  log_d("Starting setup");
/*
  #if (SOC_I2S_SUPPORTS_ADC && SOC_I2S_SUPPORTS_DAC)
    if(bits_per_sample != 16){ // ADC/DAC can only work in 16-bit sample mode
      log_e("ERROR invalid bps for ADC/DAC. Allowed only 16, requested %d", bits_per_sample);
      halt();
    }
  #else
    log_e("This chip does not support ADC / DAC mode");
    halt();
  #endif
  log_d("Checks Ok. Install driver.");
*/
  i2s_mode_t i2s_mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN);

  i2s_config_t i2s_config = {
    .mode = i2s_mode,
    .sample_rate = sample_rate,
    .bits_per_sample = bits_per_sample,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 128
  };
  // Install and start i2s driver
  if(ESP_OK != i2s_driver_install((i2s_port_t) 0, &i2s_config, 0, NULL)){
    log_e("ERROR: Failed to install i2s driver!");
    halt();
  }
  log_d("Driver installed. Setup DAC mode.");

  i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);
  log_i("Setup done");
}

void loop() {
  size_t bytes_written;
  if (count % halfWavelength == 0 ) {
    // invert the sample every half wavelength count multiple to generate square wave
    sample[0] = -1 * sample[0];
    sample[1] = sample[0];
  }
    esp_err_t ret = i2s_write((i2s_port_t)0, sample, 4, &bytes_written, 0);
    if(ret != ESP_OK){
      log_e("i2s_write error %d", ret);
    }
    //log_d("i2s_write OK (ret=%d; bytes_written=%d) sample = %d", ret, bytes_written, sample[0]);

  // increment the counter for the next sample
  count++;
}
