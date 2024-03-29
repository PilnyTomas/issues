#include <Wire.h>

#if CONFIG_IDF_TARGET_ESP32
  #define SDA_1 14
  #define SCL_1 15
#else
  #define SDA_1 10
  #define SCL_1 11
#endif
#define SLV_ADDR 0x21

void setup() {
  Serial.begin(115200);
  Serial.println("\nFor better results, turn Core Debug Level to Verbose.\n");

  // I2C MASTER Test Cases
  Serial.println("\nUC 1.1 - Master - default SDA/SCL pins, default 100KHz bus");
  Wire.end(); Wire.begin();

  Serial.println("\nUC 1.2 - Master - default SDA/SCL pins, default 100KHz bus");
  Wire.end(); Wire.begin(-1, -1);
  
  Serial.println("\nUC 1.3 - Master - default SDA/SCL pins, 1KHz bus");
  Wire.end(); Wire.begin(-1, -1, 1000);
  
  Serial.println("\nUC 1.4 - Master - default SDA, SCL pin 2, 2KHz bus");
  Wire.end(); Wire.begin(-1, 2, 2000);

  Serial.println("\nUC 1.5 - Master - default SDA/SCL pins, 400KHz bus");
  Wire.end(); Wire.begin(SDA, SCL, 400000);

#if CONFIG_IDF_TARGET_ESP32
  // Pins 6-11 are on ESP32 connected to SPI-FLASH and cannot be used for I2C, otherwise it would lead to WDT
  Serial.println("\nUC 1.6 - Master default SDA pin 12, SCL pin 13, 800KHz bus");
  Wire.end(); Wire.begin(12, 13, 800000);

  Serial.println("\nUC 1.7 - Master - SDA pin 12, SCL pin 13, default 100KHz bus");
  Wire.end(); Wire.begin(12,13);

  Serial.println("\nUC 1.8 - Master - SDA pin 14, SCL pin 15, 200KHz bus");
  Wire.end(); Wire.begin(SDA_1, SCL_1, 200000);
#else
  Serial.println("\nUC 1.6 - Master default SDA pin 8, SCL pin 9, 800KHz bus");
  Wire.end(); Wire.begin(8, 9, 800000);
  
  Serial.println("\nUC 1.7 - Master - SDA pin 8, SCL pin 9, default 100KHz bus");
  Wire.end(); Wire.begin(8,9);
  
  Serial.println("\nUC 1.8 - Master - SDA pin 10, SCL pin 11, 200KHz bus");
  Wire.end(); Wire.begin(SDA_1, SCL_1, 200000);
#endif
  Serial.println("\nUC 1.9 - Master - default SDA/SCL pins, default 100KHz bus");
  Wire.end(); Wire.begin(SDA, SCL);
  
  Serial.println("\nUC 1.10 - Master - default SDA/SCL pins, 4KHz bus");
  Wire.end(); Wire.begin(-1, -1, 4000);
  
  Serial.println("\nUC 1.11 - Master - default SDA/SCL pins, default 100KHz bus");
  Wire.end(); Wire.begin((int) SDA , (int) SCL);

  // I2C SLAVE Test Cases
  Serial.println("\nUC 2.1 - Slave - ADDR 0x28 - default SDA/SCL pins, default 100KHz bus");
  Wire.end(); Wire.begin(0x28);
  
  Serial.println("\nUC 2.2 - Slave - ADDR 0x21 - default SDA/SCL pins, default 100KHz bus");
  Wire.end(); Wire.begin(SLV_ADDR);
  
  Serial.println("\nUC 2.3 - Slave - ADDR 0x21 - default SDA/SCL pins, 4KHz bus");
  Wire.end(); Wire.begin(SLV_ADDR, -1, -1, 4000);
  
  Serial.println("\nUC 2.4 - Slave - ADDR 0x38 - SDA 10, SCL 11, default 100KHz bus");
  Wire.end(); Wire.begin(0x38, SDA_1, SCL_1, 0);
  
  Serial.println("\nUC 2.5 - Slave - ADDR 0x38 - default SDA/SCL pins, 4KHz bus");
  Wire.end(); Wire.begin(0x38, -1, SCL, 4000);

}

void loop() {
}
