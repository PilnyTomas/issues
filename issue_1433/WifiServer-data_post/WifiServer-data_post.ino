
#include "WiFi.h"
#include "ESPAsyncWebServer.h"

#define SIMULATED_SERVER_DELAY 5000

// Set your access point network credentials
const char* ssid = "foobarfoo";
const char* password = "barfoobar";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String readTemp() {
  return String(35);
  //return String(35);
}

String readHumi() {
  return String(100);
}

String readPres() {
  return String(10);
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println();
  
  // Setting the ESP as an access point
  Serial.printf("Setting AP (Access Point)\"%s\"\n", ssid);
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    delay(SIMULATED_SERVER_DELAY);
    request->send_P(200, "text/plain", readTemp().c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    delay(SIMULATED_SERVER_DELAY);
    request->send_P(200, "text/plain", readHumi().c_str());
  });
  server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest *request){
    delay(SIMULATED_SERVER_DELAY);
    request->send_P(200, "text/plain", readPres().c_str());
  });
  
  // Start server
  server.begin();
}
 
void loop(){
  
}
