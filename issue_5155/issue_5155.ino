//  Test sketch to verify the client.connected() bug in WiFiClient.cpp

/*
 * To use:
 * This sketch is designed to run on a "standard" ESP32 DEV board. Tested on one with an ESP32 WROOM with 4MB flash & 520K RAM.
 * You'll need a WiFi connection to the computer and python 2 (yeah, I know...) installed.
 * Copy the python script "stemulator.py" to your computer as well.
 * 
 *  1. Change the networkSSID[] and networkPass[] below to match your WiFi network. Lines 27 & 28.
 *  2. Change the IPAddress at line 30 to match that of your computer.
 *  3. The test is set to run once and then stop. To keep looping the test, comment out the "while (true);"
 *       statement at the bottom of loop() at line 160.
 *     Time between runs is changed by the POLL_INTERVAL at line 33.
 *  4. In your IDE, set the Core debug level to "Debug".
 *  5. Run the "stemulator.py" python script.
 *  6. Connect your board to the computer & open the Serial Monitor.
 *  7. Compile and download this sketch to your board.
 *  
 * Results are sent to the serial monitor.
*/

#include <WiFi.h>               // this is the esp32 version of this library

#define WIFI_CONNECT_TIMEOUT 60000      // # of ms to wait for a WiFi connection before aborting.


char networkSSID[] = "foo";
char networkPass[] = "bar";

//IPAddress stIP(192, 168, 31, 197);  // stemulator.py
//IPAddress stIP(74,125,115,105);     // Google
//const char* stIP = "sparkfun.com";
const char* stIP = "google.com";
                                    //
uint16_t stPort = 80;               // shouldn't have to change this, but it needs to match the port used by stemulator.py

#define POLL_INTERVAL 5000          // if the test is set to loop, # of ms between sending requests to stemulator.py
unsigned long nextPollTime;

WiFiClient stClient;

void setup() {

    // introduce ourselves
    Serial.begin(115200);
    delay(50);
    Serial.println("\r\nHey there..."); Serial.print("Running sketch: "); Serial.println(__FILE__); Serial.println("");

    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);        // set WiFi station mode
    WiFi.setSleep(false);

    int wfStatus = WiFi.status();
    unsigned long wfTimeout;

    wfTimeout = millis() + WIFI_CONNECT_TIMEOUT;

    Serial.println( "\r\nAttempting to connect to WiFi...\r\n" );
    
    WiFi.begin(networkSSID, networkPass);
    //WiFi.setSleep(false);// <--- this command disables WiFi energy save mode and eliminate connected(): Disconnected: RES: 0, ERR: 128 problem ... no it does not solve the problem

    while ( ( wfStatus != WL_CONNECTED ) && ( wfTimeout >= millis() ) ) {
        delay(250);            // pause as it takes some time for the WiFi routines to do their thing in the background
        wfStatus = WiFi.status();
    }
    
    if (wfStatus == WL_CONNECTED) {
        Serial.println("\r\nConnected to WiFi.");
    }
    else {
        WiFi.disconnect();
        Serial.println("Unable to connect to WiFi. Stopping test.");
        while (true);               // stop here if no WiFi available.
    }

    nextPollTime = millis();

}

    char replyC;
    int maxloops;

void loop() {
  while(millis() < nextPollTime);  // wait POLL_INTERVAL ms before repeating the test (after the initial run)
    Serial.println("\r\n@@@@@@@@ Start of test. At top of the test for client.\r\n");
    Serial.print("    stClient.connected() = "); Serial.println(stClient.connected());
    if (!stClient.connected()){
      
      Serial.print("\r\nAbout to call stClient.connect() with IP "); Serial.println(stIP);

      if (stClient.connect(stIP, stPort, 50000)){
        Serial.println("Return from call to stClient.connect() --> SUCCESS" );
        Serial.print("    stClient.connected() = ");
        Serial.println( stClient.connected());
//        tally.tConnect = true;
      }else{
        Serial.println( "Return from call to stClient.connect() --> FAIL\r\n" );
        Serial.println( "Stopping test." );
        while (true);
      }
    }
    
    stClient.print("GET /tally/");      // send the status request to stemulator.py
    stClient.print(3);                  // stemulator.py ignores this value, but a value here is needed 
    stClient.print("/status\r\n\r\n");  // complete the status request.

    maxloops = 0;
    while ( !stClient.available() && maxloops < 100 ) {  // Wait about a max of 10s for a reply from stemulator.py
        maxloops++;
        delay(100);                                       // wait a bit before checking for a reply from stemulator.py.
    }
    
    if(maxloops == 100){         // response from from stemulator.py timed out
        Serial.println( "Error: Response timed out." );
        Serial.println( "Stopping test." );
        while (true);
    }else{
        Serial.println( "\r\nResponse received from the server..." );
        Serial.print( "    stClient.connected() = " );
        Serial.println( stClient.connected() );

        int stcBuff = stClient.available();
    
        Serial.print( "    stClient.available() = " );
        Serial.println(stcBuff);
        Serial.print( "    stClient.connected() = " );
        Serial.println( stClient.connected() );

        Serial.println("\r\n======== About to dump the response");
    
        for (int i = 1; i <= stcBuff; i++) {
        
            replyC = stClient.read();       // pull a char out  of the rx buffer
            Serial.print(replyC);           // dump it to the Serial port
        }
    
        Serial.println("\r\n======== End of the response");
          
        Serial.print("    stClient.available() = ");
        Serial.println(stClient.available());
    
        Serial.print("\r\nAbout to call stClient.connected(). At line:"); Serial.print(__LINE__ + 2); Serial.println("\r\n");
        
        bool connectState = stClient.connected();   // !!!--> AT THIS POINT *ANY* CALL TO stClient.connected() CRASHES THE CLIENT CONNECTION <--!!!
    
        delay(250);     // without this delay, subsequent calls Serial.println() fail. Does log_d() have priority on the serial port?
        Serial.println("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ BUG ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
        Serial.println("\r\nJust called stClient.connected().");
        Serial.print("    stClient.connected() = ");
        Serial.println(connectState);

        Serial.println("\r\n@@@@@@@@ End of test.");
        
//        while(true);    // hold here or comment out to keep looping the test
    }
    
    nextPollTime = millis() + POLL_INTERVAL;

}


//  ---- EOF ----
