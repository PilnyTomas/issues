/*
Description:

softAP gets unstable when performing a wifi scan in latest 1.0.5 RC1 and RC2. In version 1.0.4 this can be done without problems.
On 1.0.5 RC1/RC2 the AP goes down while scanning.
This happens when I'm connected to the AP with a PC with Windows 10 (I tried two different laptops with Windows 10 and both have the same problem).
Curiously, on two Android phones this worked on both versions.

Sketch:

The following is the WiFi AP example with minor modifications for showing scan results.
This code compiled against 1.0.4 works perfectly but compiled against 1.0.5 RC1/RC2 does not work.
For compiling I use PlatformIO but this issue happens also with Arduino IDE.

*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

#define LED_BUILTIN 2   // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED

// Set these to your desired credentials.
const char *ssid = "helloworld2";
const char *password = "";

WiFiServer server(80);


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  Serial.println("Server started");
}

void loop() {
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Starting scan now<br>");

            uint8_t numSsid = WiFi.scanNetworks(); // Scanning Networks
            client.printf("Number of available WiFi networks discovered: %d <br>", numSsid);
            for (int i = 0; i < numSsid; i++) {
              client.printf("%d: %s, Ch:%d (%ddBm) <br>", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i));
            }

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
