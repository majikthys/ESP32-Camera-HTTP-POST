#include <WiFiClient.h>
#include "OV2640.h"
#define HTTPS_HOST              <DEFINE YOUR ADDRESS HERE, EG: "azure.com">
#define HTTPS_PORT              <DEFINE YOUR PORT INT HERE, EG: 5000>
#define WIFI_PASSWORD           <DEFINE YOUR WIFI PASSWORD HERE, EG: "bad password">
#define WIFI_SSID               <DEFINE YOUR WIFI NETWORK ID HERE, EG: "home wifi">
#define HTTPS_PORT              5000


//mime frame magic number and headers for jpg
char *request_content = "--------------------------ef73a32d42e7f04d\r\n"
                        "Content-Disposition: form-data; name=\"file\"; filename=\"%s.jpg\"\r\n"
                        "Content-Type: image/jpeg\r\n\r\n";
char *request_end = "\r\n--------------------------ef73a32d42e7f04d--\r\n";

// helper function to initialize camera. 
// Note these pins are defined for ttgo journal camera and 
// will need to be changed for other configurations
void initCamera(OV2640 cam) {
    camera_config_t camera_config;
    camera_config.ledc_channel = LEDC_CHANNEL_0;
    camera_config.ledc_timer = LEDC_TIMER_0;
    camera_config.pin_d0 = 17;
    camera_config.pin_d1 = 35;
    camera_config.pin_d2 = 34;
    camera_config.pin_d3 = 5;
    camera_config.pin_d4 = 39;
    camera_config.pin_d5 = 18;
    camera_config.pin_d6 = 36;
    camera_config.pin_d7 = 19;
    camera_config.pin_xclk = 27;
    camera_config.pin_pclk = 21;
    camera_config.pin_vsync = 22;
    camera_config.pin_href = 26;
    camera_config.pin_sscb_sda = 25;
    camera_config.pin_sscb_scl = 23;
    camera_config.pin_reset = 15;
    camera_config.xclk_freq_hz = 20000000;
    camera_config.pixel_format = CAMERA_PF_JPEG;
    camera_config.frame_size = CAMERA_FS_SVGA;

    //TODO this can stay in main
    cam.init(camera_config);
}


// helper function to get an image from camera, and then upload it as 
// http post mime body 
void updateImage(WiFiClient client, OV2640 cam)
{
    char status[64] = {0};
    char buf[1024];
    struct tm timeinfo;

    cam.run();

    if (!client.connect(HTTPS_HOST, HTTPS_PORT))
    {
        Serial.println("Connection failed");
        return;
    }

    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        snprintf(buf, sizeof(buf), request_content, String(millis()).c_str());
    }
    else
    {
        strftime(status, sizeof(status), "%Y%m%d%H%M%S", &timeinfo);
        snprintf(buf, sizeof(buf), request_content, status);
    }

    uint32_t content_len = cam.getSize() + strlen(buf) + strlen(request_end);

    String request = "POST / HTTP/1.1\r\n";
    request += "Host: "HTTPS_HOST"\r\n";
    request += "User-Agent: TTGO-Camera-Demo\r\n";
    request += "Accept: */*\r\n";
    request += "Content-Length: " + String(content_len) + "\r\n";
    request += "Content-Type: multipart/form-data; boundary=------------------------ef73a32d42e7f04d\r\n";
    request += "Expect: 100-continue\r\n";
    request += "\r\n";

    Serial.print(request);
    client.print(request);

    client.readBytesUntil('\r', status, sizeof(status));
    Serial.println(status);
    if (strcmp(status, "HTTP/1.1 100 Continue") != 0)
    {
        Serial.print("Unexpected response: ");
        client.stop();
        return;
    }

    client.print(buf);

    uint8_t *image = cam.getfb();
    size_t size = cam.getSize();
    size_t offset = 0;
    size_t ret = 0;
    while (1)
    {
        ret = client.write(image + offset, size);
        offset += ret;
        size -= ret;
        if (cam.getSize() == offset)
        {
            break;
        }
    }
    client.print(request_end);

    client.find("\r\n");

    bzero(status, sizeof(status));
    client.readBytesUntil('\r', status, sizeof(status));
    Serial.print("Response: ");
    Serial.println(status);

    //TODO Example of checking response, this must be customized to specific 
    // server responses.
    // you'll need ArduinoJSON.h and initialize to jsonDoc
    //
    // if (strncmp(status, "HTTP/1.1 200 OK", strlen("HTTP/1.1 200 OK")))
    // {
    //     Serial.print("Unexpected response: ");
    //     Serial.println(status);
    //     client.stop();
    //     return;
    // }

    // if (!client.find("\r\n\r\n"))
    // {
    //     Serial.println("Invalid response");
    // }

    // request = client.readStringUntil('\n');

    // char *str = strdup(request.c_str());
    // if (!str)
    // {
    //     client.stop();
    //     return;
    // }

    // char *start = strchr(str, '{');
    // deserializeJson(jsonDoc, start);
    // JsonObject root = jsonDoc.as<JsonObject>();
    // if (root.isNull())
    // {
    //     Serial.println("parse data fail");
    //     client.stop();
    //     free(str);
    //     return;
    // }
    // if (!strcmp((const char *)root["R"], "1"))
    // {
    //     Serial.println("Update Success");
    // }
    // free(str);
    client.stop();
}
