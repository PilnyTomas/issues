#include <WiFi.h>
#include <HTTPClient.h>
 
const char* ssid ="foo";
const char* password ="bar";

void setup(void)
{
  Serial.begin(115200);
  while(!Serial) delay(10);

  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Waiting for Wifi");
  }
 
  Serial.println("Wifi connected");
  delay(1000);
}


void loop()
{ 
  if ((WiFi.status() == WL_CONNECTED)) //Check the current connection status
  {  
    HTTPClient http;

    bool httpInitResult = http.begin("https://www.google.com");

    http.addHeader("Content-Type", "text/html");

    if( httpInitResult == false )
    {
      Serial.println("http.begin() failed!");
    }
    else
    {
      int httpCode = http.GET();
    
      Serial.print("httpCode: ");
      Serial.println(httpCode);
     
      if (httpCode > 0) //Check for the returning code
      { 
        String payload = http.getString();
    
        Serial.print("Payload: ");
        Serial.println(payload);
      }
      else 
      {
        Serial.println("Error on HTTP request");
      }
    }
    Serial.println("Trace just before http.end()");
    http.end(); //Free the resources
    Serial.println("Trace just after http.end()");
  }
  delay(1000);
}
