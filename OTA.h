#ifndef OTA_H
#define OTA_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>

const char* ssid = "RTUniverse";
const char* password = "8754820702";
const char* firmwareURL = "https://github.com/sesha07-dotcom/Industrial_Controller/releases/download/v1.0/firmware.bin";

void checkForUpdate() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  int code = 0;
  String url = firmwareURL;

  for (int attempt = 0; attempt < 3; attempt++) {
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    http.begin(client, url);

    code = http.GET();

    if (code == 302) {
      String newUrl = http.getLocation();
      http.end();
      url = newUrl;
      continue;
    }

    if (code == 200) {
      int total = http.getSize();
      Serial.printf("Firmware: %d bytes\n", total);

      if (!Update.begin(total)) {
        Serial.printf("Update.begin failed: %s\n", Update.errorString());
        http.end();
        break;
      }

      WiFiClient* stream = http.getStreamPtr();
      uint8_t buf[1024];
      size_t written = 0;
      unsigned long t = millis();

      while (written < (size_t)total && millis() - t < 60000) {
        int n = stream->read(buf, 1024);
        if (n > 0) {
          Update.write(buf, n);
          written += n;
          t = millis();
        }
      }

      if (written == (size_t)total && Update.end()) {
        Serial.println("Update success. Rebooting...");
        ESP.restart();
      } else {
        Serial.printf("OTA fail: %d/%d, %s\n", written, total, Update.errorString());
      }

      http.end();
      break;
    }

    http.end();
    break;
  }

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

#endif
