#ifndef HallSensor_h
#define HallSensor_h

#include <Arduino.h>

#define VELOCITY_MEASURE_PIN_1 16
#define VELOCITY_MEASURE_PIN_2 4
const float WHEEL_CIRCUMFERENCE = 1.4444; //[m]

class HallSensor {
public:
    HallSensor();
    void begin();
    void loop();

    float current_velocity;
    
private:
    static void addRotationSensor();
    void calculateVelocity();


    static volatile long rotation_sensor1;
    static volatile long rotations_sensor2;
    static unsigned long interrupt_time;

    unsigned long last_velocity_measure;
    unsigned long measure_time;
};

extern HallSensor hall_sensor_instance;

#endif