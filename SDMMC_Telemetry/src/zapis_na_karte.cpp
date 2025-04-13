#include <Arduino.h>
#include <SD_MMC.h>
#include <zapis_na_karte.h>
#include <ArduinoJson.h>
#include <MQTT_Communication.hpp>
#include <pomiary.h>

const int LED_PIN = 25; 
int clk = 14;
int cmd = 15;
int d0 = 2;
int d1 = 4;
int d2 = 12;
int d3 = 13;
const char* logPath = "/pomiary.txt";
File logFile;

String buffer = "";

void zapis_na_karte_init() {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    
    pinMode(d0, INPUT_PULLUP);
    pinMode(d1, INPUT_PULLUP);
    pinMode(d2, INPUT_PULLUP);
    pinMode(d3, INPUT_PULLUP);
    pinMode(cmd, INPUT_PULLUP);

    if (!SD_MMC.begin("/sdcard", true, 4000000)) {
        Serial.println("Błąd montowania karty SD");
        return;
    }
    
    digitalWrite(LED_PIN, HIGH);

    if (!SD_MMC.exists(logPath)) {
        File tmpFile = SD_MMC.open(logPath, FILE_WRITE);
        if (tmpFile) tmpFile.close();
    }

    logFile = SD_MMC.open(logPath, FILE_APPEND);
    if (!logFile) {
        Serial.println("Nie udało się otworzyć pliku do zapisu.");
        return;
    }
    
    Serial.println("Gotowy do odczytu i zapisu danych.");
}
/*
void zapis_na_karte_write(const Pomiary& pomiary) {
    if (logFile) {
        JsonDocument doc;
        doc["voltage1"] = pomiary.voltage1;
        doc["voltage2"] = pomiary.voltage2;
        doc["current1"] = pomiary.current1;
        doc["current2"] = pomiary.current2;
        doc["timestamp"] = millis();

        String jsonString;
        serializeJson(doc, jsonString);
        server_communication_instance.publishMqttMessage(jsonString); // Publish the buffer to MQTT
        buffer += jsonString + "\n"; 
    }
}
*/

void zapis_na_karte_flush() {
    if (logFile && buffer.length() > 0) {
        logFile.print(buffer);
        logFile.flush();
        buffer = "";
    }
}
