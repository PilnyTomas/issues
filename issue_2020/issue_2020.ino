/*
  You will need to upload the data folder to the SPIFFS and then connect to the SSID "Hello" and browse to 192.168.4.1.
  If you then look at the output and the developer console in Chrome you'll see the failure to download jquery...
*/

#include "StateWiFiDetails.h"

#include "SPIFFS.h"
#include <WiFi.h>

void setup() {
  
  // Set up the serial port so we can generate debug etc.
   
  Serial.begin(115200);\
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  } 

  // We are going to need access to the files stored so start up SPIFFS
  // and list out what we have.

  Serial.println("Contents of SPIFFS:");
  Serial.println("");
  if(false == SPIFFS.begin(true)){
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  // Open the root object
  
  File root = SPIFFS.open("/");

  // Loop through the files
  
  File file = root.openNextFile();
  while (file) {
    if (true == file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());    
    } else {
      Serial.print("  FILE: ");
      Serial.println(file.name());
      Serial.println(file.size());
    }

    // Move to the next file
    
    file = root.openNextFile();
  }
  
  WiFi.disconnect();    
}

void loop(){

  requestDetails();
}
