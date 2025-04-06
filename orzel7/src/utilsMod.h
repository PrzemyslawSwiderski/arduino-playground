#pragma once

bool runEvery(unsigned long &lastTime, const unsigned long interval, void (*action)());
bool isTimeAfter(unsigned long currentTime, unsigned long lastTime, const unsigned long interval);