/*
 *  Users description:
 *  I want to change the I2C clock, but I find that if the frequency
 *  exceeds 100K, the IIC frequency will not change to the size I set.
 *  For example, if I set 400K, it can only reach about 350K.
 *  Is there any way to optimize it?
 */
#include "Wire.h"

bool stay(){
  delay(5);
  if (Serial.available() > 0) {
    Serial.read();
    Serial.println("Recevied byte - continuing....");
    return false;
  }
  return true;
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  uint32_t frequency;
  int ret;
  Serial.println("TEST READY - send single byte to start");
  while(stay());

  frequency = 250000; // According to user this on the border of accurate clock setting
  ret = Wire.begin(SDA,SCL,frequency);
  Serial.printf("Wire.begin(SDA=%d, SCL=%d, frequency=%u) returned %d; frequency is set to %u\n", SDA, SCL, frequency, ret, Wire.getClock());
  Wire.beginTransmission(0x42);
  Wire.write(42);
  Wire.endTransmission();
  while(stay());
  Wire.end();

/*
  frequency = 400000; // If user is right then the output frequency will be 350kHz
  ret = Wire.begin(SDA,SCL,frequency);
  Serial.printf("Wire.begin(SDA=%d, SCL=%d, frequency=%u) returned %d; frequency is set to %u\n", SDA, SCL, frequency, ret, Wire.getClock());
  Wire.beginTransmission(0x42);
  Wire.write(42);
  Wire.endTransmission();
  while(stay());
  Wire.end();
*/

  frequency = 750000;
  ret = Wire.begin(SDA,SCL,frequency);
  Serial.printf("Wire.begin(SDA=%d, SCL=%d, frequency=%u) returned %d; frequency is set to %u\n", SDA, SCL, frequency, ret, Wire.getClock());
  Wire.beginTransmission(0x42);
  Wire.write(42);
  Wire.endTransmission();
  while(stay());
  Wire.end();

/*
  frequency = 1000000; // Maximum frequency
  ret = Wire.begin(SDA,SCL,frequency);
  Serial.printf("Wire.begin(SDA=%d, SCL=%d, frequency=%u) returned %d; frequency is set to %u\n", SDA, SCL, frequency, ret, Wire.getClock());
  Wire.beginTransmission(0x42);
  Wire.write(42);
  Wire.endTransmission();
  while(stay());
  Wire.end();
*/
  
/*
  frequency = 2000000; // Driver will set maximum frequency = 1000000
  ret = Wire.begin(SDA,SCL,frequency);
  Serial.printf("Wire.begin(SDA=%d, SCL=%d, frequency=%u) returned %d; frequency is set to %u\n", SDA, SCL, frequency, ret, Wire.getClock());
  Wire.beginTransmission(0x42);
  Wire.write(42);
  Wire.endTransmission();
  while(stay());
  Wire.end();
*/

  Serial.println("END OF TEST");
}

void loop() {
}
