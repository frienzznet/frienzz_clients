// data objects used in Frienzz
#pragma once
#include "Arduino.h"
#include "device.h"
#include "secrets.h"
#include "display.h" // for color definitions

#define MAX_NAME_LENGTH 20
#define MAX_VALUE_LENGTH (256 * 2 + 1) // max length for stored values in data objects (sha256 hex length)
#define JSON_BUFFER_SIZE (MAX_VALUE_LENGTH + 100)
#if defined(DEBUG)
#define DEVICE_PAIRING_URL_PREFIX "http://192.168.1.185:8081/#/deviceadd?d="
#else
#define DEVICE_PAIRING_URL_PREFIX "https://frienzz.net/#/deviceadd?d="
#endif

class StyleSetting
{
public: // Access specifier
    int top_color = -1;
    int right_color = -1;
    int fg_color = 0x000;
    int bg_color = 0xFFF;
    int bottom_color = -1;
    int left_color = -1;
    int top_border = 0;
    int right_border = 0;
    int bottom_border = 0;
    int left_border = 0;
    StyleSetting()
    {
        // Method/function declaration
    }
};

extern StyleSetting defaultStyleSetting;

class SymbolStyle
{       // Symbol style settings
public: // Access specifier
    uint16_t id = 0;
    StyleSetting text_style;
    StyleSetting symbol_style;
    SymbolStyle()
    { // Method/function declaration
        text_style = StyleSetting();
        symbol_style = StyleSetting();
    }
};

class SymbolStyles : public std::vector<SymbolStyle>
{
public:
    SymbolStyles()
    {
        _actual_style = SymbolStyle();
    }
    void addStyle(SymbolStyle newStyle)
    {
        push_back(newStyle);
    }

    SymbolStyle &actualStyle()
    /*
    this function returns a Style object that can be used for editing
    the actual store in the list. After editing, call storeActualStyle()
    to save the changes.


    */
    {
        return _actual_style;
    }

    SymbolStyle *getStyleById(uint16_t id)
    {
        for (std::vector<SymbolStyle>::iterator styleIterator = begin(); styleIterator != end(); ++styleIterator)
        {
            SymbolStyle &s = *styleIterator;
            if (s.id == id)
            {
                return &s;
            }
        }
        Serial.print("Style with id ");
        Serial.print(id);
        Serial.println(" not found.");
        return nullptr;
    }

    void storeActualStyle()
    {
        // store the actual friend back in the list
        addStyle(_actual_style);
        _actual_style = SymbolStyle(); // get a new friend for editing
    }

private:
    SymbolStyle _actual_style;
};

void create_default_symbol_styles();

extern SymbolStyles globalSymbolStyles; // global list of symbol styles

// in case of memory problems, consider using https://stackoverflow.com/a/54167315 ??

class Friend
{       // A single friend
public: // Access specifier
    char name[MAX_NAME_LENGTH];
    int available_time = 0;
    int symbol = 0;
    int text_color = SCHWARZ;
    int text_bg_color = WEISS;
    int symbol_color = ORANGE;
    int available_time_color = GELB;
    SymbolStyle *style;

    Friend()
    { // Method/function declaration
        set_defaults();
    }
    void setName(const char *newName)
    {
        strlcpy(name, newName, MAX_NAME_LENGTH);
    }
    char *getName()
    {
        return name;
    }

    void set_style(SymbolStyle *newStyle)
    {
        style = newStyle;
    }

    void set_defaults()
    {
        strlcpy(name, "---", MAX_NAME_LENGTH);
        available_time = 0;
        symbol = 0;
        style = globalSymbolStyles.getStyleById(0);
    }
};

class FriendsStorage : public std::vector<Friend>
{ // to avoid memory fragmentation, unused friends are stored here
public:
    FriendsStorage()
    {
    }
    void storeFriend(Friend oldFriend)
    {
        push_back(oldFriend);
    }
    Friend getFriend()
    {
        if (size() > 0)
        {
            Friend f = back();
            pop_back();
            return f;
        }
        else
        {
            Friend f;
            return f;
        }
    }
};
class FriendsList : public std::vector<Friend>
{
public:
    FriendsList()
    {
    }
    void addFriend(Friend newFriend)
    {
        push_back(newFriend);
    }
    void clear()
    {
        while (size() > 0)
        {
            Friend f = back();
            pop_back();
            store.storeFriend(f); // park the friend in the  store
        }
    }
    Friend &actualFriend()
    /*
    this function returns a Friend object that can be used for editing
    the actual friend in the list. After editing, call storeActualFriend()
    to save the changes.

    This avoids memory loss due requesting Friend objects, which are then not
    stored back in the list.

    */
    {
        return _actual_friend;
    }

    void storeActualFriend()
    {
        // update the style for the actual state

        _actual_friend.style = globalSymbolStyles.getStyleById(_actual_friend.symbol);
        // store the actual friend back in the list
        addFriend(_actual_friend);
        _actual_friend = store.getFriend(); // get a new friend for editing
    }

private:
    FriendsStorage store; // storage for unused friends
    Friend _actual_friend = store.getFriend();
};

class GlobalSettings
{
public:
    char message_type[MAX_NAME_LENGTH];
    char token[MAX_VALUE_LENGTH];
    char firmware_url[MAX_VALUE_LENGTH];
    char device_pairing_qrcode[sizeof(DEVICE_PAIRING_URL_PREFIX) + MAX_VALUE_LENGTH]; // used as flag- if contains a text, we are in the device pairing mode
    int available_time = 0;
    int available_type = 0;
    int local_seconds_of_day = 0;
    int max_seconds = 0;
    int refresh_rate_secs = 0;
    int watchface_id = 0;
    int watchface_param_id = 0;
    GlobalSettings()
    {
        strlcpy(message_type, "default", MAX_NAME_LENGTH);
        max_seconds = 28800; // 8 hours
        refresh_rate_secs = 60;
        device_pairing_qrcode[0] = 0;
        token[0] = 0;
        firmware_url[0] = 0;
        available_time = 0;
        available_type = 0;
        local_seconds_of_day = 0;
    }
    void set_message_type(const char *new_message_type)
    {
        strlcpy(message_type, new_message_type, MAX_NAME_LENGTH);
    }
    const char *get_message_type()
    {
        return message_type;
    }
    void set_token(const char *new_token)
    {
        strlcpy(token, new_token, MAX_VALUE_LENGTH);
    }
    const char *get_token()
    {
        return token;
    }
    void set_firmware_url(const char *new_firmware_url)
    {
        strlcpy(firmware_url, new_firmware_url, MAX_VALUE_LENGTH);
    }
    const char *get_firmware_url()
    {
        return firmware_url;
    }
    void set_device_pairing_qrcode(const char *new_device_pairing_qrcode)
    {
        strlcpy(device_pairing_qrcode, DEVICE_PAIRING_URL_PREFIX, MAX_VALUE_LENGTH);
        strlcat(device_pairing_qrcode, new_device_pairing_qrcode, MAX_VALUE_LENGTH);
    }
    const char *get_device_pairing_qrcode(bool with_prefix = true)
    {
        if (!with_prefix)
        {
            return device_pairing_qrcode + sizeof(DEVICE_PAIRING_URL_PREFIX) - 1;
        };

        return device_pairing_qrcode;
    }
};
