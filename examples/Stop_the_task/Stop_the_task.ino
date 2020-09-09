#include <cron.h>

#define ledPin 2 // NodeMCU & ESP 12

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);

  // Give the name of the task
  cron.add(cron::second, blink_f, "Blink");
}

void loop() {
  cron.handleEvents();
}

void blink_f() {
  digitalWrite(ledPin, !digitalRead(ledPin));

  /* Stop the task after six LED switchings, which is equal to three flashes */
  static byte flash = 0;
  flash++;
  if (flash >= 6) { // 6 switches = 3 flashes
    cron.stop("Blink");
  }
}