#ifndef OTA_H
#define OTA_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <esp_https_ota.h>

const char* ssid = "RTUniverse";
const char* password = "8754820702";
const char* VERSION = "1.0";
const char* firmwareURL = "https://github.com/sesha07-dotcom/Industrial_Controller/releases/download/v1.0/firmware.bin";
const char* versionURL = "https://raw.githubusercontent.com/sesha07-dotcom/Industrial_Controller/main/version.txt";

String fetchVersion() {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  http.begin(client, versionURL);
  int code = http.GET();
  String ver;
  if (code == 200) ver = http.getString();
  http.end();
  ver.trim();
  return ver;
}

void checkForUpdate() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  String latestVersion = fetchVersion();
  if (latestVersion.length() == 0) {
    Serial.println("No version.txt — skipping OTA");
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    return;
  }

  Serial.printf("Current: %s, Latest: %s\n", VERSION, latestVersion.c_str());

  if (latestVersion != VERSION) {
    Serial.println("New firmware available — downloading...");

    esp_http_client_config_t cfg = {};
    cfg.url = firmwareURL;
    cfg.skip_cert_common_name_check = true;
    cfg.max_redirection_count = 5;
    cfg.timeout_ms = 30000;
    cfg.keep_alive_enable = false;

    esp_https_ota_config_t ota = {};
    ota.http_config = &cfg;
    ota.bulk_size = 65536;

    esp_err_t ret = esp_https_ota(&ota);
    if (ret == ESP_OK) {
      Serial.println("Update success. Rebooting...");
      esp_restart();
    } else {
      Serial.printf("OTA failed: %s\n", esp_err_to_name(ret));
    }
  } else {
    Serial.println("Already up to date");
  }

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

#endif
