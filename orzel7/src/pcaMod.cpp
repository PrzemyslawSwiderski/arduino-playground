#include <Arduino.h>
#include "pcaMod.h"
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define WHEELS_PWM_FREQ 1000 // DC motor PWM frequency (adjust as needed)

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

static const char *TAG = "pca";
static int wheelsInUseState = 0;

static void wheelsInUse()
{
    if (!wheelsInUseState)
    {
        ESP_LOGI(TAG, "Setting use wheels mode");
        pwm.setPWMFreq(WHEELS_PWM_FREQ);
        wheelsInUseState = 1;
    }
}

void moveWheels(int channel, int speed)
{
    wheelsInUse();
    int pulse = 200 + 28 * speed;
    pwm.setPWM(channel, 0, pulse);
}

void stopWheels(int channel)
{
    pwm.setPWM(channel, 0, 0);
}

void setupPcaMod()
{
    Wire.begin(15, 14); // SDA: GPIO 15, SCL: GPIO 14
    pwm.begin();
    pwm.setOscillatorFrequency(27000000);
}
