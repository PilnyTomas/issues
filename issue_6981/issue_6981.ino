#include <Arduino.h>
/* Projects might not have full control of which libraries are included, or in which order */
extern "C" {
#include "lwip/err.h"
#include "lwip/sockets.h"
}
#include <WiFi.h>

void wifi_event_cb(WiFiEvent_t event, WiFiEventInfo_t)
{
    log_i("[WiFi-event] event: %d", event);

    switch (event) {
    case ARDUINO_EVENT_WIFI_AP_START:
        {
            IPAddress apIp(192, 168, 4, 1);
            IPAddress apNetmask(255, 255, 255, 0);

            if (!WiFi.softAPConfig(apIp, apIp, apNetmask)) {
                log_e("Failed to assign SoftAP interface");
            } else {
                log_i("softAP interface started correctly");
            }
        }
        break;
        default:
        break;
    }
}

void setup()
{
    Serial.begin(115200);

    WiFi.onEvent(wifi_event_cb);

    //WiFi.mode(WIFI_AP_STA); // why?
    //WiFi.softAP("ESP32-TEST", NULL, 1, 0);
    WiFi.softAP("ESP32-TEST"); // same as above
}

void loop()
{
    delay(1000);
}
