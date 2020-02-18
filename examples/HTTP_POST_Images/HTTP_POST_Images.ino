#include "OV2640.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include "time.h"
#include "HTTP_POST_Images_Util.h"
#include "esp_system.h"

// ##################################################################
// ## NOTE: You MUST set secret values in HTTP_POST_Images_Util.h ##
// ##################################################################

const char *ssid =              WIFI_SSID;     
const char *password =          WIFI_PASSWORD; 
const char *ntpServer =         "pool.ntp.org";
const long gmtOffset_sec =      3600;
const int daylightOffset_sec =  3600;


OV2640 cam;
//TODO make https 
// WiFiClientSecure client;
WiFiClient client;


// MAC address is a unique hex number that identifies the wifi hardware device
static String getMACAddress() 
{
  uint8_t baseMac[6];
  esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
  char baseMacChr[18] = {0};
  sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
  return String(baseMacChr);
}


void setup()
{
    Serial.begin(115200);
    while (!Serial)
    {
        ;
    }
    Serial.print("mac address: ");
    Serial.println(getMACAddress());

    initCamera(cam);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(F("."));
    }
    Serial.println(F("WiFi connected"));
    Serial.println("");
    Serial.println(WiFi.localIP());
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop()
{
    updateImage(client, cam);
    delay(30000);
}
