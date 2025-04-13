#include <Arduino.h>
#include <SPI.h>
#include <pomiary.h>
#include <ArduinoJson.h>
#include <MQTT_Communication.hpp>
#include <HallSensor.h>

#define PIN_CS1 22
#define PIN_CS2 21

const float vRef = 5.0;
const float constVoltageScalling = 0.96;
const float scaleVoltage = (10.0 + 1.2) / 1.2;

const float zeroCurrentVoltage = 2.551;
const float sensitivityACS = 0.040;
const float constCurrentScalling = 1.06;

void pomiary_init() {
    SPI.begin();
    pinMode(PIN_CS1, OUTPUT);
    pinMode(PIN_CS2, OUTPUT);
    digitalWrite(PIN_CS1, HIGH);
    digitalWrite(PIN_CS2, HIGH);
}

float adcToVoltage(uint16_t adcValue) {
    float voltage = (adcValue * vRef) / 4095.0;
    voltage *= constVoltageScalling;
    return voltage * scaleVoltage;
}

float adcToCurrent(uint16_t adcValue) {
    float currentVoltage = (adcValue * vRef) / 4095.0;
    currentVoltage *= constCurrentScalling;
    currentVoltage -= zeroCurrentVoltage;
    return currentVoltage / sensitivityACS;
}

uint16_t readADC(uint8_t pinCS, uint8_t channel) {
    if (channel > 1) return 0;
  
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    digitalWrite(pinCS, LOW);
  
    uint8_t startBit = 0b00000001;
    uint8_t configBits = 0b10000000 | (channel << 6);
  
    SPI.transfer(startBit);
    uint8_t highByte = SPI.transfer(configBits);
    uint8_t lowByte = SPI.transfer(0x00);
  
    digitalWrite(pinCS, HIGH);
    SPI.endTransaction();
  
    uint16_t result = ((highByte & 0x0F) << 8) | lowByte;
    return result;
  }

Pomiary wykonaj_pomiar(){
    Pomiary pomiar;
    uint16_t adcValue1 = readADC(PIN_CS1, 0);
    uint16_t adcValue2 = readADC(PIN_CS2, 0);
    uint16_t adcValue3 = readADC(PIN_CS1, 1);
    uint16_t adcValue4 = readADC(PIN_CS2, 1);

    pomiar.voltage1 = adcToVoltage(adcValue1);
    pomiar.voltage2 = adcToVoltage(adcValue2);
    pomiar.current1 = adcToCurrent(adcValue3);
    pomiar.current2 = adcToCurrent(adcValue4);
    pomiar.velocity1 = hall_sensor_instance.current_velocity;
    return pomiar;
}

void pomiar_do_json(const Pomiary& pomiary) {
        JsonDocument doc;
        doc["v1"] = String(pomiary.voltage1, 2);
        doc["v2"] = String(pomiary.voltage2, 2);
        doc["c1"] = String(pomiary.current1, 2);
        doc["c2"] = String(pomiary.current2, 2);
        doc["hall"] = String(pomiary.velocity1, 2);
        doc["t"] = millis();

        String jsonString;
        serializeJson(doc, jsonString);
        server_communication_instance.publishMqttMessage(jsonString);
        buffer += jsonString + "\n"; 
    }
    