#ifndef OTA_H
#define OTA_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>

const char* ssid = "RTUniverse";
const char* password = "8754820702";
const char* firmwareURL = "https://github.com/YOUR_USER/Indstrial_Controller/releases/download/v1.0/Indstrial_Controller.ino.bin"; 

void checkForUpdate() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  WiFiClientSecure client;
  client.setInsecure(); // skip cert verification for simplicity

  HTTPClient http;
  http.begin(client, firmwareURL);
  int code = http.GET();

  if (code == 200) {
    int total = http.getSize();
    WiFiClient* stream = http.getStreamPtr();

    if (Update.begin(total)) {
      size_t written = Update.writeStream(*stream);
      if (written == total && Update.end()) {
        Serial.println("Update success. Rebooting...");
        ESP.restart();
      } else {
        Serial.printf("Update failed: %s\n", Update.errorString());
      }
    }
  } else {
    Serial.printf("HTTP error: %d\n", code);
  }

  http.end();
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

#endif