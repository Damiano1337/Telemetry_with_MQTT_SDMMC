#include "HallSensor.h"

HallSensor hall_sensor_instance;

volatile long HallSensor::rotations_sensor2 = 0;
unsigned long HallSensor::interrupt_time = 0;

HallSensor::HallSensor() {}


void HallSensor::begin() {
    pinMode(VELOCITY_MEASURE_PIN_2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(VELOCITY_MEASURE_PIN_2), addRotationSensor, RISING);

    last_velocity_measure = millis();
    measure_time = 1500;
}


void HallSensor::addRotationSensor() {

    if(millis() - interrupt_time > 30){
        rotations_sensor2++;
        interrupt_time = millis();
    }  
}

void HallSensor::calculateVelocity() {
    noInterrupts();

    unsigned long currentTime = millis();
    uint8_t velocityInKilometersPerHour;

    if (last_velocity_measure + measure_time < currentTime) {

        float distance = rotations_sensor2 * WHEEL_CIRCUMFERENCE;  //distance
        float local_time = currentTime - last_velocity_measure;
        local_time = local_time/1000;  //[s]
        float velocity = distance / local_time; //[m/s]
        velocity = velocity * 3.6; // [km/h]

        last_velocity_measure = currentTime;
        rotations_sensor2 = 0;

        current_velocity = velocity;
    }

    interrupts();
}


void HallSensor::loop() {
    calculateVelocity();
}