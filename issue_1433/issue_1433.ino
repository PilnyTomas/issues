void loop(){
// wait for WiFi connection
    if((wifiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;
        //http.setTimeout(1000);
        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url
        
        if(!http.begin("http://192.168.0.101/index.html")){ //HTTP
          Serial.println("Could not connect to the server at all - there is no point of waiting on GET respond...");
          while(true);
        }
        if(http.connected()){
            Serial.println("Connected -> setTimeout");
            http.setTimeout(1000);
        }else{
            Serial.println("NOT connected -> timeout will not be set");
        }

        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int start=millis();
        int httpCode = http.GET();
        int end =millis();
        Serial.println(String("Effective timeout: ") + (end-start));
        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }

    delay(5000);
}
