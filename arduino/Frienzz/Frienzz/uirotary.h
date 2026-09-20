// UI input device, realized as a rotary encoder.
#if !defined(UIROTARY_H)
#define UIROTARY_H
#include "hardware.h"
#include "uidevice.h"
#include "uirotary.h"
#include "AiEsp32RotaryEncoder.h"
#include "Arduino.h"

// all rotary encoder code here
// uses AiEsp32RotaryEncoder library
// sample from https://github.com/igorantolic/ai-esp32-rotary-encoder/blob/master/examples/Esp32RotaryEncoderButtonOptions/Esp32RotaryEncoderButtonOptions.ino

/*
connecting Rotary encoder

Rotary encoder side    MICROCONTROLLER side
-------------------    ---------------------------------------------------------------------
CLK (A pin)            any microcontroler intput pin with interrupt -> in this example pin 32
DT (B pin)             any microcontroler intput pin with interrupt -> in this example pin 21
SW (button pin)        any microcontroler intput pin with interrupt -> in this example pin 25
GND - to microcontroler GND
VCC                    microcontroler VCC (then set ROTARY_ENCODER_VCC_PIN -1)

***OR in case VCC pin is not free you can cheat and connect:***
VCC                    any microcontroler output pin - but set also ROTARY_ENCODER_VCC_PIN 25
                        in this example pin 25

*/

// depending on your encoder - try 1,2 or 4 to get expected behaviour
// #define ROTARY_ENCODER_STEPS 1
// #define ROTARY_ENCODER_STEPS 2
#define ROTARY_ENCODER_STEPS 4

class UIRotary : public UIDevice
{
public:
    UIRotary(int ticksPerFullTurn, int numberOfInnerGears, int numberOfOuterGears, int swipeFactor = 1)
    {
        // constructor
        rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS, EXT_ROTARY_PULLUPS);
        _ticksPerFullTurn = ticksPerFullTurn;
        _numberOfInnerGears = numberOfInnerGears;
        _numberOfOuterGears = numberOfOuterGears;
        swipe_factor = swipeFactor;
    }
    ~UIRotary()
    {
        // destructor
    }
    static inline AiEsp32RotaryEncoder rotaryEncoder;

    static void IRAM_ATTR readEncoderISR()
    {
        rotaryEncoder.readEncoder_ISR();
    }

    bool button_pressed()
    {
        // implementation of button_pressed for rotary encoder
        return rotaryEncoder.isEncoderButtonDown();
    }

    bool button_release_event()
    {
        // implementation of button_release_event for rotary encoder
        // detects released button
        bool button_state = button_pressed();
        bool result = click_event && !button_state;
        click_event = button_state;
        return result;
    }

    void setup()
    {
        // implementation of setup for rotary encoder
        // we must initialize rotary encoder
        rotaryEncoder.begin();
        rotaryEncoder.setup(readEncoderISR);
        // set boundaries and if values should cycle or not
        // in this example we will set possible values between 0 and 1000;
        // rotaryEncoder.setBoundaries(_minEncoderValue, _maxEncoderValue, false);

        /*Rotary acceleration introduced 25.2.2021.
         * in case range to select is huge, for example - select a value between 0 and 1000 and we want 785
         * without accelerateion you need long time to get to that number
         * Using acceleration, faster you turn, faster will the value raise.
         * For fine tuning slow down.
         */
        // rotaryEncoder.disableAcceleration(); //acceleration is now enabled by default - disable if you dont need it
        rotaryEncoder.setAcceleration(250); // or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration
        Serial.println("rotary encoder setup done");
    };

    uint check_changes()
    // check for changes in rotary encoder position and button state, returns flag set to indicate changes
    {

        // implementation of check_changes for rotary encoder
        // int buttonState = digitalRead(ROTARY_ENCODER_BUTTON_PIN);
        // Serial.println(buttonState);
        // dont print anything unless value changed
        int rotary_delta = rotaryEncoder.encoderChanged();
        if (rotary_delta)
        {
            int new_device_value = actual_device_value + rotary_delta * _ticksPerFullTurn * _numberOfOuterGears / _numberOfInnerGears; // calculate new device value based on position change

            // check boundaries
            if (rotary_delta > 0)
            {
                if (new_device_value > max_device_value)
                {
                    new_device_value = max_device_value;
                }
            }
            else
            {
                if (new_device_value < min_device_value)
                {
                    new_device_value = min_device_value;
                }
            }
            set_device_value(new_device_value);
            Serial.print("device value: ");
            Serial.println(actual_device_value);
            _change_flags |= UIDEVICE_ANGLE_CHANGE;
            // calculate the swipe effect
            actual_swipe_position += rotary_delta * swipe_factor;
            _change_flags |= UIDEVICE_SWIPE_CHANGE;
        }
        handle_rotary_button();
        return _change_flags; // return change flags, caller can check if angle changed by checking if _change_flags & UIDEVICE_ANGLE_CHANGE is true. Caller should reset _change_flags after handling changes, for example _change_flags = 0;
    }

    // internal methods for rotary encoder button handling
    void on_button_short_click()
    {
        Serial.print("button SHORT press ");
        Serial.print(millis());
        Serial.println(" milliseconds after restart");
    }

    void on_button_long_click()
    {
        Serial.print("button LONG press ");
        Serial.print(millis());
        Serial.println(" milliseconds after restart");
    }

private:
    /* data */
    // instead of changing here, rather change numbers above
    int _ticksPerFullTurn;
    int _numberOfInnerGears;
    int _numberOfOuterGears;
    int last_swipe_position = 0;

    //********** button handling
    /*
        Note: try changing shortPressAfterMiliseconds and longPressAfterMiliseconds to fit your needs
        In case you dont need long press set longPressAfterMiliseconds=999999;  that should be enough.

        Then change what code soes on void on_button_short_click() and void on_button_long_click()
        to fit your needs

        To remove writing "+"  when button is down remove lines marked with //REMOVE THIS LINE IF YOU DONT WANT TO SEE

        Use the similar logic to implement double click or very long button press.

        There is a bit of code. But it is non-blocking.
        Try moving the rotary encoder while the button is down. You will see that it works.

        loop_rotary() is actually calling 	handle_rotary_button();
        If you prefer you can move that logic somewhere else but dont forget to call both methods frequently.
        So it is important that you have nonblocking code.

        button functions
            on_button_short_click - change function body to fit your needs
            on_button_long_click - change function body to fit your needs
            handle_rotary_button() - it already ahs logic for short and long press, but you can add double click or extra long press...
                if no need than leave it as it is (and remove lines marked with REMOVE THIS LINE IF YOU DONT WANT TO SEE)

        in case your button is reversed you can uncomment line looking like this (but not here -> do it in a handle_rotary_button):
            isEncoderButtonDown = !isEncoderButtonDown;
    */

    int positionToAngle(int position)
    {
        // implementation of positionToAngle for rotary encoder
        return (position * 360) / (_ticksPerFullTurn * _numberOfInnerGears * _numberOfOuterGears);
    }

    void handle_rotary_button()
    {
        static unsigned int lastTimeButtonDown = 0;
        static bool wasButtonDown = false;

        bool isEncoderButtonDown = rotaryEncoder.isEncoderButtonDown();
        // isEncoderButtonDown = !isEncoderButtonDown; //uncomment this line if your button is reversed

        if (isEncoderButtonDown)
        {
            // Serial.print("+"); // REMOVE THIS LINE IF YOU DONT WANT TO SEE
            if (!wasButtonDown)
            {
                // start measuring
                lastTimeButtonDown = millis();
            }
            // else we wait since button is still down
            wasButtonDown = true;
            return;
        }

        // button is up
        if (wasButtonDown)
        {
            // Serial.println(""); // REMOVE THIS LINE IF YOU DONT WANT TO SEE
            //  click happened, lets see if it was short click, long click or just too short
            if (millis() - lastTimeButtonDown >= longPressAfterMiliseconds)
            {
                on_button_long_click();
            }
            else if (millis() - lastTimeButtonDown >= shortPressAfterMiliseconds)
            {
                on_button_short_click();
            }
        }
        wasButtonDown = false;
    };
};

#endif