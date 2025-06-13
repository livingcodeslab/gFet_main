#include "StepperMotor.h"

StepperMotor::StepperMotor(AD5245 *AD, int EN_pin, int ON_OFF_pin, int HIGH_LOW_pin, int DIR_pin)
{
    _AD = AD;
    _EN = EN_pin;
    _ON_OFF = ON_OFF_pin;
    _HIGH_LOW = HIGH_LOW_pin;
    _DIR = DIR_pin;

    pinMode(_EN, OUTPUT);
    pinMode(_ON_OFF, OUTPUT);
    pinMode(_HIGH_LOW, OUTPUT);
    pinMode(_DIR, OUTPUT);

    begin();
}

/// @brief Sets up the stepper motor to be used.
bool StepperMotor::begin()
{
    bool b = _AD->begin();
    // Serial.println(b ? "true" : "false");
    // Serial.println(_AD->isConnected());
    
    if (!b)
        return false;

    enable(1);
    on(0);
    speed(0, 0);
    direction(1);

    return true;
}

/// @brief Enables or disables the stepper motor
/// @param EN_SW 1, enable; 0, disable
void  StepperMotor::enable(bool EN_SW) {
    // Serial.println(EN_SW ? "Enabled" : "Disabled");
    digitalWrite(_EN, EN_SW ? HIGH : LOW);
}

/// @brief Turns on or off the stepper motor
/// @param ON_OFF_SW 1, on; 0, off
void  StepperMotor::on(bool ON_OFF_SW) {
    // Serial.println(ON_OFF_SW ? "On" : "Off");
    digitalWrite(_ON_OFF, ON_OFF_SW ? HIGH : LOW);

    if (ON_OFF_SW) {
        // TODO: Figure out why the PWM isn't turning on the LED.
        analogWrite(LED_STATUS, 255);
    } else {
        analogWrite(LED_STATUS, 0);
    }
}

/// @brief Changes the speed profile and speed of the stepper motor
/// @param SPEED_SW 1, HIGH; 0, LOW
/// @param speed 0 - 255 (slow - fast)
void  StepperMotor::speed(bool SPEED_SW, uint8_t speed) {
    // Serial.print(SPEED_SW ? "High " : "Low ");
    // Serial.println(speed);
    digitalWrite(_HIGH_LOW, SPEED_SW ? HIGH : LOW);
    
    if (speed < 0) speed = 0;
    if (speed > 255) speed = 255;

    _AD->write(255 - speed);
    delay(100);
}

/// @brief Sets the speed profile and speed of the stepper motor based on flow rate
/// @param rate flow rate in uL/s
uint8_t  StepperMotor::speed(uint16_t rate) {
    // float uLPerR = 18;
    // float RPM = (1/uLPerR) * rate * 60;

    // int speedVal = ceil(exp((-30151 + (5000 * RPM)) / 103420));

    // // Serial.println("NEW SPEED");
    // // Serial.println(speedVal);

    // if (speedVal < 0) speedVal = 0;
    // if (speedVal > 255) speedVal = 255;

    // speed(0, speedVal);
    // return (uint8_t) speedVal;

    speed(0, rate);

    return rate;
}

/// @brief Changes the direction of the stepper motor
/// @param DIR_SW 1, forward; 0, reverse
void  StepperMotor::direction(bool DIR_SW) {
    // Serial.println(DIR_SW ? "Forward" : "Reversed");
    digitalWrite(_DIR, DIR_SW ? HIGH : LOW);
}

/// @brief Reads the set speed from the AD chip
/// @return 0 - 255 (slow - fast)
uint8_t StepperMotor::readSpeed() {
    return _AD->readDevice();
}