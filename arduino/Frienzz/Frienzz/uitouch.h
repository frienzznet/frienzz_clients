// UI input device, realized as a touch screen.
#if !defined(UITOUCH_H)
#define UITOUCH_H
#if defined(HAS_TOUCHSCREEN)
#include "hardware.h"
#include "uidevice.h"
#include "Arduino.h"
#include <Wire.h>

#include "cst816d.h" //Attention; This the modified version from https://github.com/stko/cst816d

CST816D cst816d(/* rst_pin */ TOUCH_RST, /* irq pin */ TOUCH_INT);

int touch_trigger = 0; // flag to indicate that touch event happened, can be used in main loop to check if we need to read touch coordinates and react to touch event

void callback()
{
    touch_trigger = 1;
}
class UITouch : public UIDevice
{
public:
    UITouch()
    {
    }

    ~UITouch()
    {
        // destructor
    }

    bool button_release_event()
    {
        // implementation of button_release_event for touch screen
        // detects released button
        bool result = click_event;
        click_event = false;
        return result;
    }

    void setup()
    {
        // constructor
        Wire.begin(TOUCH_I2C_SDA, TOUCH_I2C_SCL, 400000);

        if (!cst816d.begin(TFT_DIAMETER, TFT_DIAMETER))
        {
            Serial.println("Touch screen initialization failed..");
        }
        // implementation of setup for touch screen
        // we must initialize touch screen

        Serial.println("Starting uitouch setup");
        /*
                pinMode(PIN_TFT_POWER_ON, OUTPUT); // TFT poweron
                digitalWrite(PIN_TFT_POWER_ON, HIGH);
         */

        cst816d.set_rotation(TOUCH_ROTATION);
        cst816d.set_gesture_rotation(3);
        cst816d.on_touch(callback);

        // Uncomment one of these to configure when the callback function will be called :
        cst816d.mode_all(); // mode_all is the default : trigger an interrupt for both coordinates change and gestures
        // cst816d.mode_gesture(); // trigger an interrupt only for gestures
        // cst816d.mode_point(); // trigger an interrupt only for touch point coordinate change
        Serial.println("touch screen setup done");
    }

    uint check_changes()
    // check for changes in touch screen position and button state, returns flag set to indicate changes
    {
        if (touch_trigger)
        {
            touch_trigger = 0;
            last_x = cst816d.get_x();
            last_y = cst816d.get_y();
            last_gesture = cst816d.get_gesture();

            if (last_gesture != None)
            {
                Serial.print("gesture: ");
                switch (last_gesture)
                {
                case None:
                    Serial.println("None");
                    break;
                case CST816D_Gesture::Right:
                case CST816D_Gesture::Down:
                case CST816D_Gesture::Left:
                case CST816D_Gesture::Up:
                    Serial.println(last_gesture == CST816D_Gesture::Right
                                       ? "Right"
                                   : last_gesture == CST816D_Gesture::Down
                                       ? "Down"
                                   : last_gesture == CST816D_Gesture::Left
                                       ? "Left"
                                       : "Up");
                    if (isInCircle(last_x, last_y, TOUCH_ANGLE_INNER_RADIUS, TOUCH_ANGLE_OUTER_RADIUS))
                    {
                        actual_device_value = watchface_corrections(last_x, last_y);
                        _change_flags |= UIDEVICE_ANGLE_CHANGE; // set angle change flag when swipe gesture is detected, so we can check it in main loop and react to angle change event
                    }
                    break;
                case CST816D_Gesture::Click:
                    Serial.println("Click");
                    if (isInCircle(last_x, last_y, 0, TOUCH_BUTTON_DETECTION_RADIUS))
                    {
                        click_event = true; // set click event flag when click gesture is detected, so we can check it in main loop and react to click event
                    }
                    break;
                case CST816D_Gesture::DoubleClick:
                    Serial.println("DoubleClick");
                    break;
                case CST816D_Gesture::LongPress:
                    Serial.println("LongPress");
                    break;
                default:
                    Serial.println("unknown gesture");
                }
                Serial.print("x: ");
                Serial.print(last_x);
                Serial.print(" y: ");
                Serial.println(last_y);
                Serial.print("angle: ");
                Serial.println(positionToAngle(last_x, last_y));
            }
        }
        return _change_flags; // return change flags, caller can check if angle changed by checking if _change_flags & UIDEVICE_ANGLE_CHANGE is true. Caller should reset _change_flags after handling changes, for example _change_flags = 0;
    }

private:
    /* data */
    // instead of changing here, rather change numbers above
    int _ticksPerFullTurn;
    int _numberOfInnerGears;
    int _numberOfOuterGears;
    int last_swipe_position = 0;
    int last_x = 0, last_y = 0;
    CST816D_Gesture last_gesture = None;

    int positionToAngle(int touch_x, int touch_y)
    {
        // implementation of positionToAngle for touch screen
        return 180 - (atan2(touch_x - TFT_DIAMETER / 2, touch_y - TFT_DIAMETER / 2) * 180) / PI;
    }
    bool isInCircle(int touch_x, int touch_y, int inner_radius, int outer_radius)
    {
        // check if touch is within the circular area of the display, return true if it is, false otherwise
        int center_x = TFT_DIAMETER / 2;
        int center_y = TFT_DIAMETER / 2;
        int radius = TFT_DIAMETER / 2;
        int distance_squared = pow(touch_x - center_x, 2) + pow(touch_y - center_y, 2);
        return distance_squared >= pow(inner_radius, 2) && distance_squared <= pow(outer_radius, 2);
    }

    int watchface_corrections(int touch_x, int touch_y)
    {
        int new_angle = positionToAngle(touch_x, touch_y);     // calculate new device value based on touch position, convert angle to seconds of the day (assuming 12 hours full turn)
        int new_device_value = new_angle * 12 * 60 * 60 / 360; // calculate new device value based on touch position, convert angle to seconds of the day (assuming 12 hours full turn)
        Serial.print("Calculated new device value based on touch position before adjustment: ");
        Serial.println(new_device_value);
        // compare, in which watchface half the min and max device values are, and correct the new_device_value accordingly, to align it with the watchface, since the watchface has 12 hours full turn and starts at the top (12 o'clock) which is 6 hours offset from the angle calculation based on the touch position
        int section_of_day_min = (min_device_value / (12 * 60 * 60));
        int section_of_day_max = (max_device_value / (12 * 60 * 60));
        Serial.print("section_of_day_min: ");
        Serial.println(section_of_day_min);
        Serial.print("section_of_day_max: ");
        Serial.println(section_of_day_max);

        if (section_of_day_max > section_of_day_min)
        {
            if (new_angle <= 180)
            { // if max_device_value is in the first half of the day, we need to add 12 hours to the new_device_value to align it with the watchface, since the watchface has 12 hours full turn and starts at the top (12 o'clock) which is 6 hours offset from the angle calculation based on the touch position
                new_device_value += 12 * 60 * 60;
            }
        }
        if (section_of_day_min == 1)
        {
            // if min_device_value is in the second half of the day, we need to add 12 hours to the new_device_value to align it with the watchface, since the watchface has 12 hours full turn and starts at the top (12 o'clock) which is 6 hours offset from the angle calculation based on the touch position
            new_device_value += 12 * 60 * 60;
        }
        Serial.print("Calculated new device value based on touch position after adjustment: ");
        Serial.println(new_device_value);
        Serial.print("min_device_value: ");
        Serial.println(min_device_value);
        Serial.print("max_device_value: ");
        Serial.println(max_device_value);
        if (new_device_value < min_device_value)
        {
            new_device_value = min_device_value;
        }
        if (new_device_value > max_device_value)
        {
            new_device_value = max_device_value;
        }
        return new_device_value;
    }
};

#endif
#endif