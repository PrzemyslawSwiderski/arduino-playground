#include <Arduino.h>
#include "pcaMod.h"
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

#define PWM_FREQ 1000 // DC motor PWM frequency (adjust as needed)

void setupPcaMod()
{
    Wire.begin(15, 14); // SDA: GPIO 15, SCL: GPIO 14
    pwm.begin();
    pwm.setOscillatorFrequency(27000000);
    pwm.setPWMFreq(PWM_FREQ);
}

void moveWheels(int channel)
{
    // int pulse = 2408;
    int pulse = 500;
    pwm.setPWM(channel, 0, pulse); // Set Channel 1 to ~58.8% duty cycle
}

void stopWheels(int channel)
{
    pwm.setPWM(channel, 0, 0);
}
