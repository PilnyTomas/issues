
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <HardwareSerial.h>
#include "SPIFFS.h"

WebServer server(80);   
bool waitingOnData;
 
String getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}

bool handleFileRead(String path) {

  // A request has com ein for a file, we can process that here.

  Serial.println("");
  Serial.println("File requested:" + path);

  // If this is simply "/" then default to index.html
  
  if (path.endsWith("/")) 
    path += "index.html";

  // Next we need to identify the MIME type, this is done from the extension
             
  String contentType = getContentType(path);
  Serial.print("Content type: ");
  Serial.println(contentType);
  
  // Now we can try to open the file
  
  File file = SPIFFS.open(path,"r"); 
  Serial.print("File Size = ");
  Serial.println(file.size());

  // If it is there
  
  if (file) {

    // Send the file to the client
    
    size_t sent = server.streamFile<File>(file, contentType); 

    Serial.print("...");
    Serial.print(sent);
    Serial.print(" bytes sent");
    
    // Close the file, jobs done
    
    file.close();                                       
    return true;
  }

  // We did not find the file
  
  Serial.println("? File Not Found ?");
  
  return false;                                       
}

bool requestDetails()
{
  // We need to setup a Soft Access Point

  Serial.println("Setting soft-AP ... ");
  boolean result = WiFi.softAP("Hello");
  if(result == true)
  {



    // We can now configure and start the server

    server.onNotFound([]() {    

      // Try to send the file requested
      
      if (!handleFileRead(server.uri())) {

        // We have not located the file, return not found.
        
        server.send(404, "text/plain", "404: Not Found");  
      }
    });
    
    server.begin();

    waitingOnData = true;
    while(true == waitingOnData) {
      server.handleClient();
      yield();
      //delay(10);
    }

    delay(1 * 1000);

    WiFi.softAPdisconnect(true);
  }

  
  return true;
  
}
