#ifndef MQTT_CONFIG_H
#define MQTT_CONFIG_H

const char* MQTT_BROKER = "192.168.1.18"; // REPLACE WITH YOUR BACKEND/BROKER IP
const int MQTT_PORT = 1883;

const char* MQTT_TOPIC_SOIL = "smartfarm/soil";
const char* MQTT_TOPIC_PUMP_STATUS = "smartfarm/pump/status";
const char* MQTT_TOPIC_PUMP_CONTROL = "smartfarm/pump/control";

#endif
