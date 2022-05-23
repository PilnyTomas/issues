// https://github.com/espressif/arduino-esp32/issues/4423
// Connection issue with ESP32 SoftAP with Android 10 devices - why? #4423 
// When connecting newer Android Phones the Wifi connects to the ESP32,
// then drops off within a second, reconnects, then drops off in a second,
// reconnects.... and so on. My older Android phones connect no problem as do my windows laptops.


#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  
  WiFi.softAP("ESP33", "123456789");
  Serial.println();
  Serial.println("AP running");
  Serial.print("My IP address: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {}
