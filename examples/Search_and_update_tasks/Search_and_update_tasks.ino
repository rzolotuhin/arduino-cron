#include <cron.h>

#define ledPin 2 // NodeMCU & ESP 12

void setup() {
  pinMode(ledPin, OUTPUT);

  /* Give the name of the task */
  cron.add(cron::second, blink_f, "Blink");

  /* Find a task by name */
  if (cron.find("Blink")) {
    /* Set new execution interval 250 ms */
    cron.update("Blink", 250);
  }
}

void loop() {
  cron.handleEvents();
}

void blink_f() {
  digitalWrite(ledPin, !digitalRead(ledPin));
}