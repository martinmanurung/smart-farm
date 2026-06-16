#include <WiFi.h>
#include <PubSubClient.h>
#include "wifi_config.h"
#include "mqtt_config.h"
#include "relay_controller.h"

// Menggunakan pin ADC1 agar bisa bekerja bersama WiFi
#define SOIL_MOISTURE_PIN 32 

// --- PENGATURAN LOGIKA PENYIRAMAN ---
#define THRESHOLD_ON 40   // Pompa menyala otomatis jika kelembapan di bawah 40%
#define THRESHOLD_OFF 50  // Pompa mati otomatis jika kelembapan di atas 50%

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
int moisturePercentage = 0;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Menghubungkan ke ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Terhubung!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *message, unsigned int length) {
  Serial.print("Pesan masuk di topik: ");
  Serial.print(topic);
  Serial.print(". Pesan: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Menerima perintah manual dari MQTT
  if (String(topic) == MQTT_TOPIC_PUMP_CONTROL) {
    if (messageTemp == "ON") {
      Serial.println("Perintah Manual: Menyalakan Pompa via MQTT");
      turnOnPump();
      client.publish(MQTT_TOPIC_PUMP_STATUS, "{\"status\":\"ON\"}");
    } else if (messageTemp == "OFF") {
      Serial.println("Perintah Manual: Mematikan Pompa via MQTT");
      turnOffPump();
      client.publish(MQTT_TOPIC_PUMP_STATUS, "{\"status\":\"OFF\"}");
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Mencoba koneksi MQTT...");
    if (client.connect("ESP32SmartFarmClient")) {
      Serial.println("Terhubung ke Broker MQTT");
      client.subscribe(MQTT_TOPIC_PUMP_CONTROL);
    } else {
      Serial.print("Gagal, status=");
      Serial.print(client.state());
      Serial.println(" Coba lagi dalam 5 detik...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  setupRelay(); 
  setup_wifi(); 
  
  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(callback);
  client.setBufferSize(512); 
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); 

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;

    // 1. Membaca Sensor & Konversi
    int sensorValue = analogRead(SOIL_MOISTURE_PIN);
    moisturePercentage = map(sensorValue, 4095, 0, 0, 100);
    
    if (moisturePercentage < 0) moisturePercentage = 0;
    if (moisturePercentage > 100) moisturePercentage = 100;

    Serial.print("\nKelembapan Tanah: ");
    Serial.print(moisturePercentage);
    Serial.println("%");

    // 2. Publish Data Sensor
    char moistureStr[128];
    snprintf(moistureStr, sizeof(moistureStr), "{\"moisture\":%d}", moisturePercentage);
    client.publish(MQTT_TOPIC_SOIL, moistureStr);

    // 3. Tracing Log (Membantu mengecek kondisi sistem)
    Serial.print("[LOG] Set Nyala: <");
    Serial.print(THRESHOLD_ON);
    Serial.print("% | Set Mati: >");
    Serial.print(THRESHOLD_OFF);
    Serial.print("% | Pompa Sedang Nyala?: ");
    Serial.println(isPumpOn() ? "YA" : "TIDAK");

    // 4. Logika Penyiraman Otomatis
    if (moisturePercentage < THRESHOLD_ON && !isPumpOn()) {
      Serial.println("Mode Auto: Tanah terlalu kering, Pompa MENYALA.");
      turnOnPump();
      client.publish(MQTT_TOPIC_PUMP_STATUS, "{\"status\":\"ON\"}");
    } 
    else if (moisturePercentage > THRESHOLD_OFF && isPumpOn()) {
      Serial.println("Mode Auto: Tanah sudah cukup basah, Pompa MATI.");
      turnOffPump();
      client.publish(MQTT_TOPIC_PUMP_STATUS, "{\"status\":\"OFF\"}");
    }
  }
}