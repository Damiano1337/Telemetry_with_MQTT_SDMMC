#ifndef POMIARY_H
#define POMIARY_H

#include <Arduino.h>

struct Pomiary {
    float voltage1;
    float voltage2;
    float current1;
    float current2;
    float velocity1;
};

extern String buffer;

void pomiary_init();
Pomiary wykonaj_pomiar();
#endif