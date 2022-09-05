#include <network_connection.h>

static bool eth_connected = false;

IPAddress ip_address;
IPAddress gateway;
IPAddress subnet;
IPAddress dns;

void WiFiEvent(WiFiEvent_t event)
{
    switch (event)
    {
    case SYSTEM_EVENT_ETH_START:
        Serial.println(F("ETH Started"));
        // set eth hostname here
        if (!ETH.setHostname("Metrici-Radar-Eth"))
        {
            Serial.println(F("Ethernet hostname failed to configure"));
        }
        break;
    case SYSTEM_EVENT_ETH_CONNECTED:
        Serial.println(F("ETH Connected"));
        break;
    case SYSTEM_EVENT_ETH_GOT_IP:
        eth_connected = true;
        Serial.print(F("ETH MAC - "));
        Serial.print(ETH.macAddress());
        Serial.print(F(", IPv4: "));
        Serial.print(ETH.localIP());
        if (ETH.fullDuplex())
        {
            Serial.print(F(", FULL_DUPLEX"));
        }
        Serial.print(F(", "));
        Serial.print(ETH.linkSpeed());
        Serial.println(F("Mbps"));
        break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
        Serial.println(F("ETH Disconnected"));
        eth_connected = false;
        break;
    case SYSTEM_EVENT_ETH_STOP:
        Serial.println(F("ETH Stopped"));
        eth_connected = false;
        break;
    default:
        break;
    }
}

void ethConnection()
{
    ETH.begin();
    delay(50);

    if (network_settings.ip_type == "Static")
    {
        ip_address.fromString(network_settings.ip_address);
        gateway.fromString(network_settings.gateway);
        subnet.fromString(network_settings.subnet);
        dns.fromString(network_settings.dns);

        if (!ETH.config(ip_address, gateway, subnet, dns))
        {
            logOutput("WARNING: Couldn't configure STATIC IP ! Obtaining DHCP IP !");
        }
        delay(50);
    }

    int k = 0;
    while (!eth_connected && k < 20)
    {
        k++;
        delay(1000);
        Serial.println((String) "[" + k + "] - Establishing ETHERNET Connection ... ");
    }

    if (!eth_connected)
    {
        logOutput("(1) Could not connect to network ! Trying again...");
        logOutput("Radar will restart in 5 seconds !");
        restartSequence(5);
    }

    network_settings.ip_address = ETH.localIP().toString();
    network_settings.gateway = ETH.gatewayIP().toString();
    network_settings.subnet = ETH.subnetMask().toString();
    network_settings.dns = ETH.dnsIP().toString();
    // logOutput((String) "Hostname: " + ETH.getHostname());
    logOutput((String) "IP address: " + network_settings.ip_address);
    logOutput((String) "Gateway: " + network_settings.gateway);
    logOutput((String) "Subnet: " + network_settings.subnet);
    logOutput((String) "DNS: " + network_settings.dns);
}

void startConnection()
{
    if (!WiFi.mode(WIFI_STA))
    {
        logOutput("ERROR: Radar couldn't go in STA_MODE. Restarting in 5 seconds.");
        restartSequence(5);
        return;
    }

    Serial.println((String) "WiFi.getMode() [1 = STA / 2 = AP] : " + WiFi.getMode());

    WiFi.onEvent(WiFiEvent);

    if (network_settings.connection == "Ethernet")
        ethConnection();

    changed_network_config = false;
}
