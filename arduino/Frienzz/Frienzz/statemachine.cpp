#include "statemachine.h"
#include <AgileStateMachine.h>
// https://github.com/cotestatnt/AgileStateMachine/blob/main/README.md

#include "display.h"
#include "wifi_setup.h"
#include "qrtool.h"
#include "graphics.h"
#include "uidevice.h"
#include "uirotary.h"
#include "webio.h"

extern char token[MAX_VALUE_LENGTH];
extern char connect[MAX_VALUE_LENGTH];
extern UIDevice *uidevice;
extern FriendsList globalFriendsList;
extern GlobalSettings globalSettings; // global settings object

/* The Finite State Machine */
StateMachine fsm;

State *st_main_state;
State *st_pairing_state;
State *st_change_state_state;

bool button_release_ev = false;
int16_t pixel_size_value;
int16_t actual_pixel_pos;
int beginning_state_change_type;
int beginning_state_change_time;
int actual_state_change_type;
int actual_state_change_time;
unsigned long st_change_state_last_change_time;
bool this_is_the_first_loop = true;

void main_screen_on_enter()
{
    pixel_size_value = pixel_size();
    actual_pixel_pos = 0;
    Serial.print("Pixel size value: ");
    Serial.println(pixel_size_value);
    draw_background();
    loop_webio(true);
    draw_user_state(globalSettings.available_type, globalSettings.available_time, false);
}

void main_screen_on_run()
{
    if (loop_webio(false))
    {
        for (std::vector<Friend>::iterator friendIterator = globalFriendsList.begin(); friendIterator != globalFriendsList.end(); ++friendIterator)
        {
            Friend f = *friendIterator;
            Serial.print("Friend Name: ");
            Serial.print(f.getName());
            Serial.print(", available_time: ");
            Serial.println(f.available_time);
        }
        Serial.print("token :");
        Serial.println(globalSettings.get_token());
        Serial.print("device_pairing_qrcode :");
        Serial.println(globalSettings.get_device_pairing_qrcode());
        Serial.print("message_type :");
        Serial.println(globalSettings.get_message_type());
        Serial.print("available_time :");
        Serial.println(globalSettings.available_time);
        Serial.print("local_seconds_of_day");
        Serial.println(globalSettings.local_seconds_of_day);
        Serial.print("available_type :");
        Serial.println(globalSettings.available_type);
        Serial.print("max_seconds :");
        Serial.println(globalSettings.max_seconds);
        Serial.print("refresh_rate_secs :");
        Serial.println(globalSettings.refresh_rate_secs);

        draw_user_state(globalSettings.available_type, globalSettings.available_time, false);
        if (strlen(globalSettings.get_device_pairing_qrcode(false)) > 0)
        {
            Serial.println("Device Pairing QR Code detected, switching to pairing state");
            fsm.setCurrentState(st_pairing_state);
        }
    }

    pixel_size_value = pixel_size();
    uidevice->check_changes();
    int swipe_position = uidevice->get_swipe_position();
    while (swipe_position != 0 && pixel_size_value > 0)
    {
        actual_pixel_pos += swipe_position;
        drawFriendsList(actual_pixel_pos, true);
        uidevice->check_changes();
        swipe_position = uidevice->get_swipe_position();
    }
    // draw_symbol(1, ROT, true);
    // draw_symbol(1, GELB, false);
    if (actual_pixel_pos >= pixel_size_value)
    {
        actual_pixel_pos = 0;
        // set_min_device_value(actual_pixel_pos);
    }
    if (pixel_size_value > 0)
    {
        drawFriendsList(actual_pixel_pos, true);
        actual_pixel_pos += SCROLL_STEP_WIDTH;
        if (actual_pixel_pos >= pixel_size_value)
            actual_pixel_pos = 0;
        // set_min_device_value(actual_pixel_pos);
    }
    delay(MAIN_DISPLAY_DELAY);
}

void pairing_screen_on_enter()
{
    qr_drawQRCode(globalSettings.get_device_pairing_qrcode(), 4, WEISS);
    Serial.print("Pair URL :");
    Serial.println(globalSettings.get_device_pairing_qrcode());
}

void pairing_screen_on_run()
{
    if (loop_webio(false))
    {
        qr_drawQRCode(globalSettings.get_device_pairing_qrcode(), 4, WEISS);
        Serial.print("Pair URL :");
        Serial.println(globalSettings.get_device_pairing_qrcode());
    }
    if (button_release_ev)
    {
        Serial.println("Device Pairing button released");
        loop_webio(true); // force a server update to check if pairing is done
    }
    if (strlen(globalSettings.get_device_pairing_qrcode(false)) == 0) // ok, pairing done
    {
        Serial.println("Device Pairing done, switching to main state");
        fsm.setCurrentState(st_main_state);
    }
}

void change_state_screen_on_enter()
{
    actual_state_change_type = globalSettings.available_type;
    beginning_state_change_type = globalSettings.available_type;
    actual_state_change_time = globalSettings.available_time;
    beginning_state_change_time = globalSettings.available_time;
    st_change_state_last_change_time = millis();
    this_is_the_first_loop = true;
    uidevice->set_device_value(globalSettings.local_seconds_of_day + actual_state_change_time);
    uidevice->set_min_device_value(globalSettings.local_seconds_of_day);
    uidevice->set_max_device_value(globalSettings.local_seconds_of_day + globalSettings.max_seconds);
    draw_background();
    SymbolStyle *available_type_style = globalSymbolStyles.getStyleById(actual_state_change_type);
    if (actual_state_change_type < 1 || available_type_style == nullptr)
    {
        Serial.print("WARNING: Correct Unknown actual_state_change_type : ");
        Serial.println(actual_state_change_type);
        actual_state_change_type = 1;
    }
    draw_user_state(actual_state_change_type, actual_state_change_time, true);
    draw_version_info();
}

void change_state_screen_on_run()
{
    // check for timeout
    bool change_state_timeout = (millis() - st_change_state_last_change_time) > CHANGE_STATE_BUTTON_TIMEOUT * 1000;
    if (change_state_timeout)
    {
        fsm.setCurrentState(st_main_state);
        return;
    }
    if (button_release_ev)
    {
        st_change_state_last_change_time = millis();

        actual_state_change_type++;
        SymbolStyle *available_type_style = globalSymbolStyles.getStyleById(actual_state_change_type);
        if (available_type_style == nullptr) // we reached end of available states
        {
            if (this_is_the_first_loop)
            {
                // in case there is only one state available,
                // we should not exit immediately
                this_is_the_first_loop = false;
                actual_state_change_type = 1; // reset to first available state
            }
            else
            {
                Serial.print("End of available states reached, go back to main");
                actual_state_change_type = -1;
                fsm.setCurrentState(st_main_state);
                // to avoid an unwanted user state change
                // when leaving this state because
                // of this state "overrun", we'll
                // need to "mark" this situation somehow
                return;
            }
        }
        Serial.print("available_type_style : ");
        Serial.println((long long unsigned int)available_type_style);
    }
    int uidevice_change_state = uidevice->check_changes();
    if (uidevice_change_state & UIDEVICE_ANGLE_CHANGE)
    {
        st_change_state_last_change_time = millis();
        actual_state_change_time = uidevice->get_device_value() - globalSettings.local_seconds_of_day;
    }
    if (uidevice_change_state & UIDEVICE_ANGLE_CHANGE || button_release_ev)
    {
        draw_user_state(actual_state_change_type, actual_state_change_time, true);
        Serial.print("actual_state_change_type : ");
        Serial.println(actual_state_change_type);
        Serial.print("actual_state_change_time : ");
        Serial.println(actual_state_change_time);
    }
}

void change_state_screen_on_exit()
{
    Serial.print("Running change_state_screen_on_exit() with actual_state_change_type of : ");
    Serial.println(actual_state_change_type);

    if ((actual_state_change_type != -1) && (beginning_state_change_time != actual_state_change_time || beginning_state_change_type != actual_state_change_type))
    {
        Serial.println("Updating user state on server");
        bool success = webio_jsonrpc_send_status(actual_state_change_type, actual_state_change_time);
    }
}

void setup_state_machine()
{
    st_main_state = fsm.addState("Main Screen", main_screen_on_enter, nullptr, main_screen_on_run);
    st_pairing_state = fsm.addState("Pairing Screen", pairing_screen_on_enter, nullptr, pairing_screen_on_run);
    st_change_state_state = fsm.addState("Change Stage Screen", change_state_screen_on_enter, change_state_screen_on_exit, change_state_screen_on_run);
    st_main_state->addTransition(st_pairing_state, []() { // A lambda callback function will be used as trigger
        return (strlen(globalSettings.get_token()) == 0);
    });
    st_main_state->addTransition(st_change_state_state, button_release_ev);
    fsm.setInitialState(st_main_state);
    fsm.start();
    Serial.print("Active state: ");
    Serial.println(fsm.getActiveStateName());
    Serial.println();
}

void loop_state_maschine()
{
    // Read inputs
    button_release_ev = uidevice->button_release_event();
    // Run State Machine	(true is state changed)
    if (fsm.execute())
    {
        Serial.print(F("Active state: "));
        Serial.println(fsm.getActiveStateName());
    }
}