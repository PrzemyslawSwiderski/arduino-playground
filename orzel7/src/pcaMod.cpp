#include <Arduino.h>
#include "pcaMod.h"
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define WHEELS_MIN_PULSE 200
#define WHEELS_MAX_PULSE 1500
#define WHEELS_SPEED_MULTIPLIER 20
#define WHEELS_PWM_FREQ 1000 // DC motor PWM frequency (adjust as needed)

#define SERVO_MIN_PULSE 320  // Minimum pulse length (out of 4096)
#define SERVO_MAX_PULSE 340  // Maximum pulse length (out of 4096)
#define SERVO_FREQ 50 // Analog servos typically run at ~50 Hz

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

static const char *TAG = "pca";
static int wheelsInUseState = 0;

static void wheelsInUse()
{
    if (!wheelsInUseState)
    {
        ESP_LOGI(TAG, "Setting wheels mode");
        pwm.setPWMFreq(WHEELS_PWM_FREQ);
        wheelsInUseState = 1;
    }
}

static void headInUse()
{
    if (wheelsInUseState)
    {
        ESP_LOGI(TAG, "Setting head mode");
        pwm.setPWMFreq(SERVO_FREQ);
        wheelsInUseState = 0;
    }
}

void moveWheels(int channel, int speedPercent)
{
    wheelsInUse();
    speedPercent = constrain(speedPercent, 0, 100); // Ensure 0–100
    int pulse = map(speedPercent, 0, 100, WHEELS_MIN_PULSE, WHEELS_MAX_PULSE);
    pwm.setPWM(channel, 0, pulse);
}

void stopWheels(int channel)
{
    pwm.setPWM(channel, 0, 0);
}

void moveHead(int channel, int percent)
{
    headInUse();
    percent = constrain(percent, 0, 100);           // Ensure 0–100
    int pulse = map(percent, 0, 100, SERVO_MIN_PULSE, SERVO_MAX_PULSE);
    ESP_LOGD(TAG, "HEAD PULSE: %d", pulse);
    pwm.setPWM(channel, 0, pulse);
}

void setupPcaMod()
{
    Wire.begin(15, 14); // SDA: GPIO 15, SCL: GPIO 14
    pwm.begin();
    pwm.setOscillatorFrequency(27000000);
}
