#include <I2S.h>

I2SClass I2Sfoo(1, 0, PIN_I2S1_SD, PIN_I2S1_SCK, PIN_I2S1_FS); // default - half duplex

void setup() {
  Serial.begin(115200);
  while(!Serial) delay(10);
  
  log_d("SOC_I2S_NUM == %d", SOC_I2S_NUM);

  //I2S.begin(I2S_PHILIPS_MODE, 16000, 24);
  //I2S1.begin(I2S_PHILIPS_MODE, 16000, 24);
  I2Sfoo.begin(I2S_PHILIPS_MODE, 16000, 24);

  while(1) delay(1000);

}

void loop() {

}
