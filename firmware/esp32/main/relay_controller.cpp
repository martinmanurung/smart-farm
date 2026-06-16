#include "relay_controller.h"

// Variabel untuk melacak status pompa saat ini
bool pumpState = false; 

void setupRelay() {
  // Menggunakan trik Open Drain untuk Relay 5V Low-Level Trigger
  pinMode(RELAY_PIN, OUTPUT_OPEN_DRAIN);
  
  // Pastikan pompa mati saat pertama kali menyala (Active Low: HIGH = Mati)
  digitalWrite(RELAY_PIN, HIGH);
  pumpState = false;
}

void turnOnPump() {
  digitalWrite(RELAY_PIN, LOW); // LOW untuk menyalakan relay
  pumpState = true;
}

void turnOffPump() {
  digitalWrite(RELAY_PIN, HIGH); // HIGH untuk mematikan relay
  pumpState = false;
}

bool isPumpOn() {
  return pumpState;
}