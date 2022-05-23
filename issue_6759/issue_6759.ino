#include <WiFi.h>
#include <WebServer.h>

WebServer server(80); 

void setup(){
server.on("", HTTP_GET, handle);
server.on("/", HTTP_GET, handle);
server.begin();
}

void loop(){
server.handleClient();
}

void handle(){
Serial.println(server.args());
}
