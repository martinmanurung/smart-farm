#ifndef RELAY_CONTROLLER_H
#define RELAY_CONTROLLER_H

#include <Arduino.h>

#define RELAY_PIN 25 // Pin untuk Relay

void setupRelay();
void turnOnPump();
void turnOffPump();
bool isPumpOn();

#endif