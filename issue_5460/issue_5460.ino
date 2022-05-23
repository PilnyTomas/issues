#include <WiFi.h>
#include <WiFiMulti.h>

float CpuTemperature(void) {
  log_d("Temp function...");
  #ifdef CONFIG_IDF_TARGET_ESP32
    log_d("ESP32");
    return (float)temperatureRead();  // In Celsius
  /*
    // These jumps are not stable either. Sometimes it jumps to 77.3
    float t = (float)temperatureRead();  // In Celsius
    if (t > 81) { t = t - 27.2; }        // Fix temp jump observed on some ESP32 like DualR3
    return t;
  */
  #else
//    #ifndef CONFIG_IDF_TARGET_ESP32S3
      // Currently (20210801) repeated calls to temperatureRead() on ESP32C3 and ESP32S2 result in IDF error messages
      static float t = NAN;
      if (isnan(t)) {
        t = (float)temperatureRead();  // In Celsius
      }
      return t;
/*
    #else
      log_d("This is not ESP32 or it IS ESP32S3");
      return NAN;
    #endif
*/
  #endif

}

WiFiMulti wifiMulti;
void setup() {
  Serial.begin(115200);
  delay(250);

  for(int i = 0; i <= 5; ++i){
    Serial.printf("Temp = %f\n",CpuTemperature());
    delay(1000);
  }


/*
  Serial.printf("Turning on WiFi to rise chip temperature\n");

  wifiMulti.addAP("foo", "bar");
  wifiMulti.addAP("hello", "");
  wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");

  Serial.println("Connecting Wifi...");
  if(wifiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
  */

  delay(1000);
  Serial.printf("Temp = %f\n",CpuTemperature());
  delay(1000);
}

void loop() {
  Serial.printf("Temp = %f\n",CpuTemperature());
  delay(1000);
  if(wifiMulti.run() != WL_CONNECTED) {
    Serial.println("WiFi not connected!");
  }
}
