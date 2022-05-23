#include <ESPAsyncWebServer.h>

extern const char WIFI_SSID[] = "foo";
extern const char WIFI_PASS[] = "bar";

uint64_t start = 0;
AsyncWebServer server(80);

void onWiFiGotIp(WiFiEvent_t event, WiFiEventInfo_t eventInfo) {
  Serial.print("IP Address: ");
  Serial.println((IPAddress) eventInfo.got_ip.ip_info.ip.addr);

  Serial.print("WiFi ready after ");
  Serial.print(millis() - start);
  Serial.println("ms.");
}

void onWiFiDisonnected(WiFiEvent_t event) {
  Serial.println("WiFi Disconnected");
  WiFi.disconnect(true);
  delay(100);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
}

void setupWebServer() {
  server.on("/index.html", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "OK");
  });

  server.onNotFound([] (AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not Found");
  });

  server.begin();
}

void setup() {
  start = millis();
  Serial.begin(115200);
  Serial.println("setup start");

  WiFi.disconnect(true);
  WiFi.onEvent(onWiFiGotIp, ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(onWiFiDisonnected, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  setupWebServer();
  Serial.println("setup end");
}

void loop() {
  delay(100);
}
