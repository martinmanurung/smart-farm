#ifndef RELAY_CONTROLLER_H
#define RELAY_CONTROLLER_H

#define RELAY_PIN 26 // Change to your actual relay pin

bool pumpState = false;

void setupRelay() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Assume LOW is OFF
}

void turnOnPump() {
  digitalWrite(RELAY_PIN, HIGH); // Assume HIGH is ON
  pumpState = true;
}

void turnOffPump() {
  digitalWrite(RELAY_PIN, LOW);
  pumpState = false;
}

bool isPumpOn() {
  return pumpState;
}

#endif
