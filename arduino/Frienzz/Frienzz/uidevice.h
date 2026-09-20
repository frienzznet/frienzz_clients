// UI input device, which can come either from a rotary encoder or from a touchsreen.
#pragma once
#include "Arduino.h"
#include "device.h"
#include "secrets.h"
#include "display.h" // for color definitions

#define MAX_NAME_LENGTH 20
#define MAX_VALUE_LENGTH (256 * 2 + 1) // max length for stored values in data objects (sha256 hex length)
#define JSON_BUFFER_SIZE (MAX_VALUE_LENGTH + 100)
#define UIDEVICE_ANGLE_CHANGE 1
#define UIDEVICE_SWIPE_CHANGE 2
// in case of memory problems, consider using https://stackoverflow.com/a/54167315 ??

class UIDevice
{       // A virtual generic UI input device, which can come either from a rotary encoder or from a touchsreen.
public: // Access specifier
    UIDevice()
    { // Method/function declaration
        ;
    }
    virtual bool button_release_event() = 0;
    const int get_device_value()
    // get the current value of the device in seconds of the day
    {
        int result = actual_device_value; // this an anle
        Serial.print("get_device_value() : ");
        Serial.println(result);
        Serial.print("min_device_value : ");
        Serial.println(min_device_value);
        Serial.print("max_angle : ");
        Serial.println(max_device_value);
        _change_flags &= ~UIDEVICE_ANGLE_CHANGE;
        return result;
    }
    const int get_swipe_position()
    {
        int result = actual_swipe_position;
        actual_swipe_position = 0;
        _change_flags &= ~UIDEVICE_SWIPE_CHANGE;
        return result;
    }
    void set_device_value(int value)
    {
        actual_device_value = value;
    }
    void set_min_device_value(int value) { min_device_value = value; }
    void set_max_device_value(int value) { max_device_value = value; }

    virtual void setup() = 0;
    virtual uint check_changes() = 0;

protected:
    unsigned int shortPressAfterMiliseconds = 50;  // how int short press shoud be. Do not set too low to avoid bouncing (false press events).
    unsigned int longPressAfterMiliseconds = 1000; // how long čong press shoud be.
    int min_device_value = 0;
    int max_device_value = 0;
    int actual_device_value = 0;
    int actual_swipe_position = 0;
    int swipe_factor = 1;   // how much the swipe effect should be for each unit of rotary change, can be used to make the swipe effect more or less sensitive. For example, if you want to have a stronger swipe effect, you can set it to 2, so each unit of rotary change will result in 2 units of swipe position change.
    uint _change_flags = 0; // bitmask for changes, for example if angle changed, we can set _change_flags |= UIDEVICE_ANGLE_CHANGE; and check it later in main loop to see what changed and react accordingly. This is useful to avoid doing heavy calculations when there is no change in input device.
    bool click_event = false;
};
