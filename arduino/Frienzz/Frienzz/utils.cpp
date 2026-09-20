#include "utils.h"

bool intervalElapsed(unsigned long *previousMicros, unsigned long intervalMicros)
{
    unsigned long currentMicros = micros();
    if (*previousMicros == 0) // first call
    {
        *previousMicros = currentMicros;
        return true;
    }

    if (currentMicros - *previousMicros >= intervalMicros)
    {
        *previousMicros = currentMicros;
        return true;
    }
    return false;
}

uint secondsToAngle(uint seconds)
{
    // Convert seconds to angle (0-360) for 12h format, so 5 minutes (300 seconds) is the max time, which corresponds to 360 degrees. So we scale seconds to fit into 360 degrees.
    uint result = ((seconds * 360) / (60 * 60 * 12)) % 360; // 5 minutes is the max time, so we scale to 360 for full rotation
    Serial.print("secondsToAngle() : seconds : ");
    Serial.print(seconds);
    Serial.print(", result : ");
    Serial.println(result);
    return result;
}

int angleToSeconds(uint start_seconds, uint angle)
{
    // Convert angle (0-360) back to seconds
    Serial.print("angleToSeconds() : start_seconds : ");
    Serial.print(start_seconds);
    Serial.print(", angle : ");
    Serial.print(angle);
    int result = ((angle + start_seconds / (60 * 60 * 12) * 360) * 60 * 60 * 12) / 360; // scale back from angle to seconds
    Serial.print(", result : ");
    Serial.println(result);
    return result;
}