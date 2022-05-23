#include <WiFi.h>

const char* ssid = "foo";
const char* ssid2 = "hotspot";
const char* password = "bar";

bool myWiFiFirstConnect = true;

const char* decode(wl_status_t state){
  switch(state){
   case  255: return "WL_NO_SHIELD"; break;
    case  0: return "WL_IDLE_STATUS"; break;
    case  1: return "WL_NO_SSID_AVAIL"; break;
    case  2: return "WL_SCAN_COMPLETED"; break;
    case  3: return "WL_CONNECTED"; break;
    case  4: return "WL_CONNECT_FAILED"; break;
    case  5: return "WL_CONNECTION_LOST"; break;
    case  6: return "WL_DISCONNECTED"; break;
    default : return " non-existing option"; break;
  }
}

void myWiFiTask(void *pvParameters) {
  wl_status_t state;

  while (true) {
    state = WiFi.status();
    Serial.print("state = "); Serial.print(decode(state)); Serial.print(" ("); Serial.print(state); Serial.println(")");
    if (state != WL_CONNECTED) {  // We have no connection
      if (state == WL_NO_SHIELD) {  // WiFi.begin wasn't called yet
        Serial.println("Connecting WiFi");

        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);

      } else if (state == WL_CONNECT_FAILED) {  // WiFi.begin has failed (AUTH_FAIL)
        Serial.println("Disconnecting WiFi");

        WiFi.disconnect(true);

      } else if (state == WL_DISCONNECTED) {  // WiFi.disconnect was done or Router.WiFi got out of range
        if (!myWiFiFirstConnect) {  // Report only once
          myWiFiFirstConnect = true;

          Serial.println("WiFi disconnected");
        }
      }

      vTaskDelay (250); // Check again in about 250ms

    } else { // We have connection
      Serial.print("Connected...");
      if (myWiFiFirstConnect) {  // Report only once
        myWiFiFirstConnect = false;

        Serial.print("Connected to ");
        Serial.println(ssid);
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        Serial.println("");
      }

      vTaskDelay (5000); // Check again in about 5s
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Create a connection task with 8kB stack on core 0
  //xTaskCreatePinnedToCore(myWiFiTask, "myWiFiTask", 8192, NULL, 3, NULL, 0);
}

void loop() {
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("1st connection try...");
    WiFi.begin(ssid, password);
    //WiFi.begin(ssid2, password);
  }
  //Wait for connection -> success = webserver start
  if (WiFi.waitForConnectResult() == WL_CONNECTED){
      Serial.println("WiFi connected.");
    //Webserver_init();
    delay(1000);
  }else{
    Serial.print("failed; 2nd connection try...");
    WiFi.disconnect(true);
    delay(1000);

    //WiFi.begin(ssid, password);
    WiFi.begin(ssid2, password);
    if (WiFi.waitForConnectResult() == WL_CONNECTED){
      Serial.println("Connected on 2nd try");
      //Webserver_init();
    }else{
      Serial.println("Wifi not connected");
      Serial.println("WebServer won´t be started");
    }
  }
}
