#include "OTA.h"

const int relays[] = {23, 22, 21, 19, 18, 5};
const int relaycount = 6;
const int doorSensor = 4;

unsigned long lowStartTime = 0;
bool wasLow = false;
int lastState = 0;

void setRelays(int lo1, int lo2) {
  for (int i = 0; i < relaycount; i++) {
    int p = relays[i];
    digitalWrite(p, (p == lo1 || p == lo2) ? LOW : HIGH);
  }
}

void setNormalState() {
  lastState = 0;
  setRelays(21, -1);
}

void setup() {
  Serial.begin(115200);
  pinMode(doorSensor, INPUT_PULLUP);

  checkForUpdate();

  for (int i = 0; i < relaycount; i++)
    pinMode(relays[i], OUTPUT);

  setNormalState();
}

void loop() {
  int sensor = digitalRead(doorSensor);
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 2000) {
    lastPrint = millis();
    Serial.print("Sensor: ");
    Serial.print(sensor == LOW ? "LOW" : "HIGH");
    Serial.print("  wasLow: ");
    Serial.print(wasLow);
    Serial.print("  lastState: ");
    Serial.println(lastState);
  }

  if (sensor == LOW) {
    if (!wasLow) {
      wasLow = true;
      lowStartTime = millis();
      Serial.println("Sensor went LOW - timer started");
    }

    unsigned long t = millis() - lowStartTime;

    if (t >= 60000 && lastState != 2) {
      lastState = 2;
      Serial.println("60s reached: 21+23 ON");
      setRelays(21, 23);
    } else if (t >= 30000 && lastState != 1) {
      lastState = 1;
      Serial.println("30s reached: 22 ON");
      setRelays(22, -1);
    }
  } else if (wasLow) {
    wasLow = false;
    lowStartTime = 0;
    Serial.println("Sensor went HIGH - reset to normal");
    setNormalState();
  }
}