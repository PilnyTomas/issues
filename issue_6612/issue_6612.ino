#include <WiFiClientSecure.h>

const char* ssid     = "foo";     // your network SSID (name of wifi network)
const char* password = "bar";     // your network password

const char*  server = "www.howsmyssl.com";  // Server URL


WiFiClientSecure client;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while(!Serial){log_d(".");}
  Serial.println("Serial: Starting");
  log_d("log: Starting");
  delay(10000);

  esp_sleep_enable_timer_wakeup(10e7);  // 10^7 us = 10 s

  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  log_d("log: Attempting to connect to SSID: %s", ssid);
  WiFi.begin(ssid, password);

  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    // wait 1 second for re-trying
    delay(1000);
  }

  Serial.print("Connected to ");
  Serial.println(ssid);

  Serial.println("\nStarting connection to server...");
  client.setInsecure();//skip verification
  if (!client.connect(server, 443))
    Serial.println("Connection failed!");
  else {
    Serial.println("Connected to server!");
    // Make a HTTP request:
    client.println("GET https://www.howsmyssl.com/a/check HTTP/1.0");
    client.println("Host: www.howsmyssl.com");
    client.println("Connection: close");
    client.println();

    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }
    // if there are incoming bytes available
    // from the server, read them and print them:
    while (client.available()) {
      char c = client.read();
      Serial.write(c);
    }

    client.stop();

    Serial.println("Serial: Entering light sleep");
    log_d("log: Entering light sleep");

    esp_light_sleep_start();

    Serial.println("Serial: Leaving light sleep and entering deep sleep");
    log_d("log: Leaving light sleep and entering deep sleep");

    esp_deep_sleep_start();

    Serial.println("Serial: Leaving deep sleep");
    log_d("log: Leaving deep sleep");
  }
}

void loop() {
  // do nothing
}
