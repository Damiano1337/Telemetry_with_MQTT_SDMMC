#include <Arduino.h>
#include <pomiary.h>
#include <zapis_na_karte.h>
#include <MQTT_Communication.hpp>
#include <HallSensor.h>

unsigned long lastMeasurementTime = 0;
unsigned long lastFlushTime = 0;

void setup() {
  Serial.begin(115200);

  server_communication_instance.begin();

  delay(5000);

  hall_sensor_instance.begin();
  pomiary_init();
  zapis_na_karte_init();
}

void loop() {

  if (!MQTT_client.connected()) server_communication_instance.mqttReconnect(); // check if MQTT client is connected
  if (WiFi.status() != WL_CONNECTED) server_communication_instance.wifiReconnect();

  unsigned long now = millis();

  if (now - lastMeasurementTime >= 1) { 
    lastMeasurementTime = now;
    Pomiary pomiary = wykonaj_pomiar();
    pomiar_do_json(pomiary); // Dodajemy pomiar do bufora
  }

  if (now - lastFlushTime >= 20) {
    lastFlushTime = now;
    zapis_na_karte_flush(); // Wysyłamy bufor na kartę SD
  }
}
