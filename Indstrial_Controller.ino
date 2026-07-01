#include "OTA.h"

const int relays[] = {23, 22, 21, 19, 18, 5};
const int relaycount = 6;
const int doorSensor = 4;

unsigned long trigStartTime = 0;
bool wasTriggered = false;
int lastState = 0;
int lastReading = LOW;
unsigned long lastChangeTime = 0;
const int debounceMs = 50;

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
  // Set relays OFF immediately (prevents boot-time chatter)
  for (int i = 0; i < relaycount; i++) {
    pinMode(relays[i], OUTPUT);
    digitalWrite(relays[i], HIGH);
  }

  Serial.begin(115200);
  pinMode(doorSensor, INPUT_PULLDOWN);

  checkForUpdate();

  setNormalState();
  Serial.println("Ready");
}

void loop() {
  int reading = digitalRead(doorSensor);

  if (reading != lastReading)
    lastChangeTime = millis();
  lastReading = reading;

  if (millis() - lastChangeTime < debounceMs)
    return;

  int sensor = reading;

  if (sensor == HIGH) {
    if (!wasTriggered) {
      wasTriggered = true;
      trigStartTime = millis();
      Serial.println("Door closed - timer started");
    }

    unsigned long t = millis() - trigStartTime;

    if (t >= 60000 && lastState != 2) {
      lastState = 2;
      Serial.println("60s: 21+23 ON");
      setRelays(21, 23);
    } else if (t >= 30000 && lastState == 0) {
      lastState = 1;
      Serial.println("30s: 22 ON");
      setRelays(22, -1);
    }
  } else if (wasTriggered) {
    wasTriggered = false;
    trigStartTime = 0;
    Serial.println("Door open - reset");
    setNormalState();
  }
}