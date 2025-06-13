#pragma once

#include <Arduino.h>
#include <AD5245.h>
#include "PinMapping.h"

class StepperMotor {
    public:
        explicit StepperMotor(AD5245 *AD, int EN_pin, int ON_OFF_pin, int HIGH_LOW_pin, int DIR_pin);

        bool begin();

        void enable(bool EN_SW);

        void on(bool ON_OFF_SW);

        void speed(bool SPEED_SW, uint8_t speed);
        uint8_t speed(uint16_t rate);

        // void speed(bool RUN_TOGGLE);

        void direction(bool DIR_SW);

        uint8_t readSpeed();

    private:
        AD5245* _AD;

        int rampVol = 10; // uL
        int rampTimne = 300; // ms

        int _EN;
        int _ON_OFF;
        int _HIGH_LOW;
        int _DIR;

};