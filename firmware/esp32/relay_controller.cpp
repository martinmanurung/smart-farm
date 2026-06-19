#include "relay_controller.h"

bool pumpState = false;

void setupRelay() {
    pinMode(RELAY_PIN, OUTPUT);

    // Relay OFF
    digitalWrite(RELAY_PIN, HIGH);

    pumpState = false;
}

void turnOnPump() {

    Serial.println("===== RELAY ON =====");

    digitalWrite(RELAY_PIN, LOW);

    pumpState = true;
}

void turnOffPump() {

    Serial.println("===== RELAY OFF =====");

    digitalWrite(RELAY_PIN, HIGH);

    pumpState = false;
}

bool isPumpOn() {
    return pumpState;
}