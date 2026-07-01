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
  if (digitalRead(doorSensor) == LOW) {
    if (!wasLow) {
      wasLow = true;
      lowStartTime = millis();
    }

    unsigned long t = millis() - lowStartTime;

    if (t >= 60000 && lastState != 2) {
      lastState = 2;
      setRelays(21, 23);  // 21 ON, 23 ON
    } else if (t >= 30000 && lastState != 1) {
      lastState = 1;
      setRelays(22, -1);  // 22 ON
    }
  } else if (wasLow) {
    wasLow = false;
    setNormalState();
  }
}