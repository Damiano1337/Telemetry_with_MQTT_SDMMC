#ifndef MQTT_COMMUNICATION_HPP
#define MQTT_COMMUNICATION_HPP

#include <WiFiClientSecure.h>
#include <MQTTClient.h>   //AWS old
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"


/**** Secure WiFi Connectivity Initialisation *****/
extern WiFiClientSecure espClient;


/**** MQTT Client Initialisation Using WiFi Connection *****/
extern PubSubClient MQTT_client;


class ServerCommunication{

public:
    void begin();
    void setupWifi();

    void mqttReconnect();
    void wifiReconnect();

    void publishMqttMessage(String payload);

private:
    const char* wifi_ssid = "ESP32test"          ;
    const char* wifi_password = "maslo123";

    const char* mqtt_server = "6aa9c1de47b0404e82629f1961517374.s1.eu.hivemq.cloud";
    const char* mqtt_username = "dataServer";
    const char* mqtt_password = "Hydrive1";
    const uint16_t mqtt_port = 8883;
    const char* mqtt_publish_topic = "esp32/pub";
};

extern ServerCommunication server_communication_instance;

#endif