/*
 *  This sketch tries to Connect to the best AP based on a given list
 *
 */

#include <WiFi.h>
#include <WiFiMulti.h>

WiFiMulti wifiMulti;

time_t start;
void setup()
{
    Serial.begin(115200);
    delay(10);

    wifiMulti.addAP("foo", "bar");
    wifiMulti.addAP("bar", "foo");
    wifiMulti.addAP("helloworld", "");
    wifiMulti.addAP("helloworld2", "");

    Serial.println("Connecting Wifi...");
    start = millis();
    if(wifiMulti.run() == WL_CONNECTED) {
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        Serial.print("Time to connect = "); Serial.print(millis() - start); Serial.println(" ms");
        Serial.println("Restart in 3 sec...");
        delay(3000);
        Serial.println("Restarting");
        Serial.println("===================================================================");
        ESP.restart();
   }
   Serial.println("Did not connect on first run. Try to connect in lop with 10ms delays");
}

void loop()
{
  if(wifiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.print("Time to connect = "); Serial.print(millis() - start); Serial.println(" ms");
    Serial.println("Restart in 3 sec...");
    delay(3000);
    Serial.println("Restarting");
    Serial.println("===================================================================");
    ESP.restart();
  }
  delay(10);
  Serial.print(".");
  
}
