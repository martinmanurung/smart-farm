#include "mqtt_config.h"
#include "relay_controller.h"
#include "wifi_config.h"
#include <PubSubClient.h>
#include <WiFi.h>

#define SOIL_MOISTURE_PIN 34 // Analog pin for soil moisture sensor
#define THRESHOLD 40         // % Moisture threshold to auto turn on pump

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (String(topic) == MQTT_TOPIC_PUMP_CONTROL) {
    if (messageTemp == "ON") {
      Serial.println("Turn ON pump via MQTT");

      turnOnPump();

      client.publish(MQTT_TOPIC_PUMP_STATUS, "{\"status\":\"ON\"}");
    } else if (messageTemp == "OFF") {
      Serial.println("Turn OFF pump via MQTT");

      turnOffPump();

      client.publish(MQTT_TOPIC_PUMP_STATUS, "{\"status\":\"OFF\"}");
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32SmartFarmClient")) {
      Serial.println("connected");
      client.subscribe(MQTT_TOPIC_PUMP_CONTROL);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(callback);
  client.setBufferSize(512); // Menambah ukuran buffer internal MQTT

  setupRelay();
}

long lastMsg = 0;
int moisturePercentage = 0;

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;

    // Read sensor (values depend on calibration, let's say 0-4095)
    int sensorValue = analogRead(SOIL_MOISTURE_PIN);

    // Map to percentage (Assuming 4095 is dry, 0 is wet. Adjust as per your
    // sensor)
    moisturePercentage = map(sensorValue, 4095, 0, 0, 100);
    if (moisturePercentage < 0)
      moisturePercentage = 0;
    if (moisturePercentage > 100)
      moisturePercentage = 100;

    Serial.print("Soil Moisture: ");
    Serial.print(moisturePercentage);
    Serial.println("%");

    // Publish moisture
    char moistureStr[128];
    snprintf(moistureStr, sizeof(moistureStr), "{\"moisture\":%d}",
             moisturePercentage);
    client.publish(MQTT_TOPIC_SOIL, moistureStr);

    // Auto control logic
    if (moisturePercentage < THRESHOLD && !isPumpOn()) {
      Serial.println("Auto mode: Moisture low, turning ON pump.");
      turnOnPump();
      client.publish(MQTT_TOPIC_PUMP_STATUS, "{\"status\":\"ON\"}");
    } else if (moisturePercentage > (THRESHOLD + 20) && isPumpOn()) {
      Serial.println("Auto mode: Moisture high enough, turning OFF pump.");
      turnOffPump();
      client.publish(MQTT_TOPIC_PUMP_STATUS, "{\"status\":\"OFF\"}");
    }
  }
}
