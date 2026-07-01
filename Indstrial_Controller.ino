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
  Serial.begin(115200);
  pinMode(doorSensor, INPUT_PULLDOWN);

  checkForUpdate();

  for (int i = 0; i < relaycount; i++)
    pinMode(relays[i], OUTPUT);

  setNormalState();
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
    }

    unsigned long t = millis() - trigStartTime;

    if (t >= 60000 && lastState != 2) {
      lastState = 2;
      setRelays(21, 23);
    } else if (t >= 30000 && lastState != 1) {
      lastState = 1;
      setRelays(22, -1);
    }
  } else if (wasTriggered) {
    wasTriggered = false;
    trigStartTime = 0;
    setNormalState();
  }
}