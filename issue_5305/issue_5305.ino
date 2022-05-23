// https://github.com/espressif/arduino-esp32/issues/5305

#include <ETH.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>

#define SOFT_AP

#define ETH_ADDR        1
#define ETH_POWER_PIN   5
#define ETH_MDC_PIN     23
#define ETH_MDIO_PIN    18
#define ETH_TYPE        ETH_PHY_IP101

// ETH variables
IPAddress ethIp(192, 168, 1, 18);
IPAddress gateway(192, 168, 1, 2);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns1(192, 168, 1, 10);
IPAddress dns2(192, 168, 1, 20);

// WiFi variables
const char *apSsid = "WIFI_AP";
IPAddress apIp(192, 168, 1, 19);

#define PORT_1 80

AsyncWebServer asyncWebServer(PORT_1);

static bool eth_connected = false;
    
void setupETH()
{
  // Start the ethernet
  ETH.begin(ETH_ADDR, ETH_POWER_PIN, ETH_MDC_PIN, ETH_MDIO_PIN, ETH_TYPE);
  ETH.config(ethIp, gateway, subnet);
}

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

void setHtmlCallback()
{
  asyncWebServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Run call back");
    request->send(200, "text/plain", "Hello world");
  });
  
  asyncWebServer.onNotFound(notFound);
}

void WiFiEvent(WiFiEvent_t event)
{
  switch (event) {
    case SYSTEM_EVENT_ETH_START:
      Serial.println("ETH Started");
      //set eth hostname here
      ETH.setHostname("esp32-ethernet");
      break;
    case SYSTEM_EVENT_ETH_CONNECTED:
      Serial.println("ETH Connected");
      Serial.print("ETH IP: ");
      Serial.println(ETH.localIP());
      break;
    case SYSTEM_EVENT_ETH_GOT_IP:
      Serial.print("ETH MAC: ");
      Serial.print(ETH.macAddress());
      Serial.print(", IPv4: ");
      Serial.print(ETH.localIP());
      if (ETH.fullDuplex()) {
        Serial.print(", FULL_DUPLEX");
      }
      Serial.print(", ");
      Serial.print(ETH.linkSpeed());
      Serial.println("Mbps");
      eth_connected = true;
      break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH Disconnected");
      eth_connected = false;
      break;
    case SYSTEM_EVENT_ETH_STOP:
      Serial.println("ETH Stopped");
      eth_connected = false;
      break;
    default:
      break;
  }
}

void setup()
{
  Serial.begin(115200);

  WiFi.onEvent(WiFiEvent);
  

#ifdef SOFT_AP
  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSsid);
  WiFi.softAPConfig(apIp, gateway, subnet);
#endif

  setupETH();
  
  // Start the Ethernet web server
  setHtmlCallback();
  asyncWebServer.begin();

  Serial.print("WiFi MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.print("ETH MAC: ");
  Serial.println(ETH.macAddress());
}

void loop()
{
}
