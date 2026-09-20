#ifndef UTILS_H
#define UTILS_H
#include <Arduino.h>
bool intervalElapsed(unsigned long *previousMicros, unsigned long intervalMicros);
uint secondsToAngle(uint seconds);
int angleToSeconds(uint start_seconds, uint angle);
#endif // UTILS_H