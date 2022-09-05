#include <Arduino.h>
#define BUFFER_SIZE 256
HardwareSerial HWS{1};

void setup() {
  Serial.begin(115200);
    while(!Serial){delay(10);}
  Serial.printf("Serial ready.\n");
#if CONFIG_IDF_TARGET_ESP32
  HWS.begin(9600, 12, 13); // Default pins for ESP32 conflict with SPI FLASH resulting in WDT
#else
  HWS.begin(9600);
#endif
  Serial.printf("Available for write before setting up buffer: %d\n", HWS.availableForWrite());
  HWS.end();

  HWS.setTxBufferSize(BUFFER_SIZE);
#if CONFIG_IDF_TARGET_ESP32
  HWS.begin(9600, 12, 13); // Default pins for ESP32 conflict with SPI FLASH resulting in WDT
#else
  HWS.begin(9600);
#endif
  Serial.printf("Available for write after setting up buffer of size %d: %d\n", BUFFER_SIZE, HWS.availableForWrite());
}

void loop() {
  delay(1000);
}
