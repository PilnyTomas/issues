#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "foobarfoo";
const char* password = "barfoobar";

//Your IP address or domain name with URL path

const char* serverNameTemp = "http://192.168.4.1/temperature";
/*
const char* serverNameHumi = "http://192.168.4.1/humidity";
const char* serverNamePres = "http://192.168.4.1/pressure";
*/
/*
const char* serverNameTemp = "http://foo/bar";
const char* serverNameHumi = "http://foo/barbar";
const char* serverNamePres = "http://foo/barfoobar";
*/
//const char* serverNameTemp = "http://foo/bar";
const char* serverNameHumi = "http://foo/humidity";
const char* serverNamePres = "http://192.168.4.1/bar";

String temperature;
String humidity;
String pressure;

unsigned long previousMillis = 0;
const long interval = 5000; 

void setup() {
  Serial.begin(115200);
  
  Serial.printf("Connecting to AP (Access Point)\"%s\"\n", ssid);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillis >= interval) {
    // Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED ){
      //temperature = httpGETRequest(serverNameTemp);
      humidity = httpGETRequest(serverNameHumi);
      //pressure = httpGETRequest(serverNamePres);
      Serial.println("Temperature: " + temperature + " *C - Humidity: " + humidity + " % - Pressure: " + pressure + " hPa");
      
      // save the last HTTP GET Request
      previousMillis = currentMillis;
    }
    else {
      Serial.println("WiFi Disconnected");
      delay(1000);
    }
  }
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;

  //client.setTimeout(10);
  // Your Domain name with URL path or IP address with path
  Serial.printf("Trying to connect to server \"%s\"\n", serverName);
  if(!http.begin(client, serverName)){
    Serial.println("Could not connect to the server at all - there is no point of waiting on GET respond...");
    while(true);
  }else{
    Serial.printf("Connected to server \"%s\"\n", serverName);
  }

  uint16_t timeout = 3000;
  http.setTimeout(timeout);
  Serial.printf("HTTP timeout set to %d ms\n", timeout);
  
  Serial.println("HTTP called");
  // Send HTTP POST request
  time_t start = millis();
  int httpResponseCode = http.GET();
  Serial.printf("###############################\n");
  Serial.printf("HTTP GET returned after %lu ms\n", millis()-start);
  Serial.printf("###############################\n");
  
  String payload = "--"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
    delay(5000);
  }
  // Free resources
  http.end();

  return payload;
}
