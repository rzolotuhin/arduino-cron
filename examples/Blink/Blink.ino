#include <cron.h>

#define ledPin 2 // NodeMCU & ESP 12

void setup() {
  pinMode(ledPin, OUTPUT);

  cron.add(cron::second, blink_f);
}

void loop() {
  cron.handleEvents();
}

void blink_f() {
  digitalWrite(ledPin, !digitalRead(ledPin));
}