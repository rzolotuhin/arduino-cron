#include <cron.h>

#define ledPin 2 // NodeMCU & ESP 12

void setup() {
  Serial.begin(115200);
  Serial.println();

  pinMode(ledPin, OUTPUT);

  /* Main task */
  cron.add(cron::second, blink_f, "Blink");
  /* Control task */
  cron.add(cron::time_5s, control_f);
  /* Changing the state for the task "Blink" */
  cron.add(cron::time_10s, switch_f);
}

void loop() {
  cron.handleEvents();
}

void blink_f() {
  digitalWrite(ledPin, !digitalRead(ledPin));
}

void control_f() {
  if (cron.isActive("Blink")) {
    Serial.println(F("Task \"Blink\" is active"));
  } else {
    Serial.println(F("Task \"Blink\" is not active"));
  }
}

void switch_f() {
  if (cron.isActive("Blink")) {
    cron.stop("Blink");
    Serial.println(F("The task is stopped"));
  } else {
    cron.update("Blink", cron::time_1s); // You can set any interval
    Serial.println(F("The task is resumed"));
  }
}