#include "utilsMod.h"
#include "Arduino.h"

bool runEvery(unsigned long &lastTime, const unsigned long interval, void (*action)())
{
  unsigned long currentTime = millis();
  if (isTimeAfter(currentTime, lastTime, interval))
  {
    action();
    lastTime = currentTime;
    return true;
  }
  return false;
}

bool isTimeAfter(unsigned long currentTime, unsigned long lastTime, const unsigned long offsetTime)
{
  if ((currentTime >= lastTime && currentTime - lastTime >= offsetTime) || (currentTime < lastTime && (0xFFFFFFFF - lastTime + currentTime + 1) >= offsetTime))
  {
    return true;
  }
  else
  {
    return false;
  }
}
