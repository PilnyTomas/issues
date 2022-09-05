/*
 * This is an example to read analog data at high frequency using the I2S peripheral
 * Run a wire between pins 27 & 32
 * The readings from the device will be 12bit (0-4096) 
 */
#include <driver/i2s.h>

#define I2S_SAMPLE_RATE 32000 // scanning frequency must be at least twice the scanned signal to satisfy Nyquist
#define ADC_INPUT ADC1_CHANNEL_4 //pin 32

// PWM
#define PWM_BIT_RESOLUTION 12
#define PWM_DUTY (1^PWM_BIT_RESOLUTION)/2 // 50%
#define PWM_FREQUENCY 5000
#define OUTPUT_PIN 27

uint16_t adc_reading;

void i2sInit()
{
   i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
    .sample_rate =  I2S_SAMPLE_RATE,              // The format of the signal using ADC_BUILT_IN
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // is fixed at 12bit, stereo, MSB
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 8,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
   };
   i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
   i2s_set_adc_mode(ADC_UNIT_1, ADC_INPUT);
   i2s_adc_enable(I2S_NUM_0);
}

void reader(void *pvParameters) {
  //i2sInit();
  uint32_t read_counter = 0;
  uint64_t read_sum = 0;
// The 4 high bits are the channel, and the data is inverted
  uint16_t offset = (int)ADC_INPUT * 0x1000 + 0xFFF;
  size_t bytes_read;
  uint16_t buffer[i2s_config*bits_per_sample/8] = {0};
  while(1){
    //log_d("i2s_read(sizeof(buffer) == %d)",sizeof(buffer));
    i2s_read(I2S_NUM_0, &buffer, sizeof(buffer), &bytes_read, 15);
    //log_d("bytes_read == %d", bytes_read);
    Serial.printf("%d  %d\n", offset - buffer[0], offset - buffer[1]);
    if (bytes_read == sizeof(buffer)) {
      read_sum += offset - buffer[0];
      read_sum += offset - buffer[1];
      read_counter++;
    } else {
      Serial.println("buffer empty");
    }
    //log_d("if(read_counter(%d) == I2S_SAMPLE_RATE(%d))", read_counter, I2S_SAMPLE_RATE);
    if (read_counter == I2S_SAMPLE_RATE) {
      log_d("if(read_counter(%d) == I2S_SAMPLE_RATE(%d))", read_counter, I2S_SAMPLE_RATE);
      adc_reading = read_sum / I2S_SAMPLE_RATE / 2;
      //Serial.printf("avg: %d millis: ", adc_reading);
      //Serial.println(millis());
      read_counter = 0;
      read_sum = 0;
      i2s_adc_disable(I2S_NUM_0);
      i2s_adc_enable(I2S_NUM_0);
      Serial.printf("ADC reading: %d\n", adc_reading);
    }
  }
}

void setup() {
  Serial.begin(115200);
  // Put a signal out on pin 
  while(!Serial){delay(10);}
  log_d("setup PWM frequency %d and with resolution %d bits", PWM_FREQUENCY, PWM_BIT_RESOLUTION);
  uint32_t freq = ledcSetup(0, PWM_FREQUENCY, PWM_BIT_RESOLUTION);
  if(!freq){
    log_d("PWM setup failed. Halt!");
    while(true){;}
  }
  Serial.printf("Output frequency: %d\n", freq);
  ledcAttachPin(OUTPUT_PIN, 0); // pin 27
  Serial.printf("PWM duty: %d\n", PWM_DUTY);
  ledcWrite(0, PWM_DUTY);
  // Initialize the I2S peripheral
  log_d("3");
  i2sInit();
  // Create a task that will read the data
  log_d("4");
  xTaskCreatePinnedToCore(reader, "ADC_reader", 2048, NULL, 1, NULL, 1);
  log_d("5");
}

void loop() {
}
