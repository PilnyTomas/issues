/*
Hello.
I have the same problem.
I am looking for a solution to this problem.

Pin usage:
32: i2s_adc(input)
33: pwm_10kHz(output)
26: analog(input)

Pin Connection
Variable resistor on 26
Connect 32 and 33

Environments:
Arduino 1.8.15
Espressif Systems version 2.0.2 (latest version shown in Board Manager)
Board used: ESP32-DevKitC-32E (4MB)

If you comment out "int unsigned value = analogRead(26);" in "dmaRead()", the pulse is output normally.
Otherwise, the output will only be 4096. (code below)
*/

#include <driver/i2s.h>

#define I2S_SAMPLE_RATE 20000

  #define ADC_INPUT ADC1_CHANNEL_4 //pin 32

void i2sInit(){
  i2s_config_t i2s_config = {
  .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
  //.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
  .sample_rate = I2S_SAMPLE_RATE,
  .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
  .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
  .communication_format = I2S_COMM_FORMAT_STAND_I2S,
  .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
  .dma_buf_count = 4,
  .dma_buf_len = 32,
  .use_apll = false,
  .tx_desc_auto_clear = false,
  .fixed_mclk = 0
  };


  //int raw_out;
  //Serial.printf("BEFORE I2S INSTALL\n");
  //Serial.printf("ADC1_CH7 analogRead(35) = %d\n", analogRead(35));
  //Serial.printf("ADC2_CH9 analogRead(26) = %d\n", analogRead(26)); // Without this the I2S reads junk
  //Serial.printf("pin 35 ADC1_CH7 adc1_get_raw() = %d\n", adc1_get_raw(ADC1_CHANNEL_7));
  //adc2_get_raw(ADC2_CHANNEL_9, ADC_WIDTH_BIT_12, &raw_out);
  //Serial.printf("pin 26 ADC2_CH9 adc2_get_raw() = %d\n", raw_out);


  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_adc_mode(ADC_UNIT_1, ADC_INPUT);
  i2s_adc_enable(I2S_NUM_0);
}

void dmaRead(){
  //I2S
  uint16_t buffer[64];
  size_t bytes_read;

  i2s_read(I2S_NUM_0, &buffer, sizeof(buffer), &bytes_read, 15);

  Serial.printf("i2s_read %d bytes\n", bytes_read);
  for(int i = 0; i < bytes_read/sizeof(uint16_t); i ++){
    Serial.printf("i2s_read: buffer[%d]=%d\n", i, buffer[i]&0xFFF);
    //Serial.printf("[%d]=%d\n", i, buffer[i]&0xFFF);
  }

  //Analog
  int raw_out;
  Serial.printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  //Serial.printf("ADC1_CH7 analogRead(35) = %d\n", analogRead(35));
  //Serial.printf("ADC2_CH9 analogRead(26) = %d\n", analogRead(26));
  //Serial.printf("pin 35 ADC1_CH7 adc1_get_raw() = %d\n", adc1_get_raw(ADC1_CHANNEL_7));
  adc2_get_raw(ADC2_CHANNEL_9, ADC_WIDTH_BIT_12, &raw_out);
  Serial.printf("pin 26 ADC2_CH9 adc2_get_raw() = %d\n", raw_out);
  Serial.printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  Serial.printf("\n");

}

void setup() {
//Serial
Serial.begin(115200);

//PWM
pinMode(33, OUTPUT);
ledcSetup(0, 5000, 8);
ledcAttachPin(33, 0);
delay(100);
ledcWrite(0, 128);

//I2S
i2sInit();

//analogRead
//pinMode(26, ANALOG); // ADC2_CH9
//pinMode(35, ANALOG); // ADC1_CH7
}

void loop() {
dmaRead();
delay(1000);
}
