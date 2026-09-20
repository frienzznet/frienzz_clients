#pragma once

#include "JsonHandler.h"

#include "dataobjects.h"
#include "configfs.h"

extern FriendsList globalFriendsList;   // global list of friends
extern SymbolStyles globalSymbolStyles; // global list of symbol styles

extern GlobalSettings globalSettings; // global settings object

// Virtual Handler class implementation
class WebIOHandler : public JsonHandler
{

public:
    /* Process the value provided at a specific JSON path / element
     * This forms the core operation of the Handler.
     */

    bool equals_path(ElementPath *path, const char *expectedPath, bool leading_path_only)
    /*

    memory safe comparison of ElementPath to expectedPath string
    returns true if they are equal, false otherwise

    if leading_path_only is true, only the leading part of the path is compared,
    i.e. if expectedPath is "message.config", then paths like
    "message.config.status.friends.available_type"
    will also match.

    if leading_path_only is false, the full path must match exactly.

    root elements need to be named explicitly as paths, e.g. "token" or "connect"

    this routine avoids memory allocation issues by not building
    a full string representation of the ElementPath, but instead
    compares each element one by one against the expectedPath string.

    */
    {
        int count = path->getCount();
        if (count <= 0)
            return false;
        int nr_of_elements_to_check = 0;
        for (int index = 0; index < count; index++)
        {
            ElementSelector *selector = path->get(index);
            const char *name = selector->getKey();
            if (*name == '\0') // selectors with an index only do not have a name
            {
                continue;
            }
            nr_of_elements_to_check++;
        }
        if (leading_path_only)
        {
            // in leading path only mode, we only check up to the last full element
            nr_of_elements_to_check--;
        }
        if (nr_of_elements_to_check < 1)
            return false;

        // Serial.print("Comparing path: ");
        // print_path(path);
        // Serial.print(" to expected: ");
        // Serial.print(expectedPath);
        // Serial.print(" with a nr_of_elements_to_check of ");
        // Serial.println(nr_of_elements_to_check);

        for (int index = 0; index < count; index++)
        {
            ElementSelector *selector = path->get(index);
            const char *name = selector->getKey();
            if (*name == '\0')
            {
                // Serial.print("[");
                // Serial.print(selector->getIndex());
                // Serial.print("]");
                continue;
            }
            char seperator = '.';

            char *key_char = (char *)selector->getKey();
            // Serial.print("Comparing path element '");
            for (; *key_char != '\0'; key_char++)
            {
                // Serial.print(*key_char);
                if (*key_char != *expectedPath)
                {
                    // Serial.print("Path mismatch at index ");
                    // Serial.print(index);
                    // Serial.print(": expected '");
                    // Serial.print(*expectedPath);
                    // Serial.print("' but got '");
                    // Serial.print(*key_char);
                    // Serial.println("'");
                    return false;
                }
                expectedPath++;
            }
            // Serial.println();
            nr_of_elements_to_check--;
            if (nr_of_elements_to_check == 0) // that was the last element to check
            {
                // last element

                if (*expectedPath != '\0')
                {
                    // Serial.print("Path length mismatch at end: expected end but got '");
                    // Serial.print(*expectedPath);
                    // Serial.println("'");
                    return false;
                }
                return true;
            }
            else
            {
                if (*expectedPath != '.' && *expectedPath != '[')
                {
                    // Serial.print("Leading path mismatch at end: expected '.', '[' but got '");
                    // Serial.print(*expectedPath);
                    // Serial.println("'");
                    return false;
                }
                expectedPath++;
            }
        }
    }

    void
    print_path(ElementPath *path)
    /*
    print a path
    memory safe comparison of ElementPath to expectedPath string
    returns true if they are equal, false otherwise

    this routine avoids memory allocation issues by not building
    a full string representation of the ElementPath, but instead
    prints each element one by one

    */
    {
        int count = path->getCount();
        if (count <= 0)
            return;

        for (int index = 0; index < count; index++)
        {
            ElementSelector *selector = path->get(index);
            const char *name = selector->getKey();
            if (*name == '\0')
            {
                Serial.print("[");
                Serial.print(selector->getIndex());
                Serial.print("]");
                continue;
            }
            if (index > 0)
            {
                Serial.print(".");
            }
            Serial.print(selector->getKey());
        }
    }

    void value(ElementPath path, ElementValue value)
    {
        // For debug only.
        // Serial.print("Value Path: ");
        // print_path(&path);
        // Serial.println();
        Friend &actualFriend = globalFriendsList.actualFriend();
        if (actualFriend.style == nullptr)
        {
            actualFriend.set_style(globalSymbolStyles.getStyleById(0));
            // BUG style normally must not be null
        }
        if (equals_path(&path, "config.status.friends", true))
        {
            // Serial.println("MATCH for: config.status.friends");
            if (!strcmp(path.getKey(), "friend_nickname"))
            {
                actualFriend.setName(value.getString());
            }
            else if (!strcmp(path.getKey(), "available_time"))
            {
                actualFriend.available_time = value.getInt();
            }
            else if (!strcmp(path.getKey(), "available_type"))
            {
                actualFriend.symbol = value.getInt();
            }
        }
        else if (equals_path(&path, "config.device_pairing_qrcode", false)) // root elements need to be named explicitly as paths, e.g. "token" or "connect"
        {
            // Serial.println("MATCH for: config.device_pairing_qrcode");

            globalSettings.set_device_pairing_qrcode(value.getString());
            globalSettings.set_token(value.getString()); // during the pairing, we use the qrcode as token
        }
        else if (equals_path(&path, "token", false)) // root elements need to be named explicitly as paths, e.g. "token" or "connect"
        {
            // Serial.println("MATCH for: token");

            bool new_token = strlen(value.getString()) > 0 || strcmp(globalSettings.get_token(), value.getString()) != 0;
            globalSettings.set_token(value.getString());
            globalSettings.set_device_pairing_qrcode("");
            if (new_token)
            {
                Serial.print("Save new Token: ");
                Serial.println(globalSettings.get_token());
                configfs_save();
            }
        }
        else if (equals_path(&path, "type", false)) // root elements need to be named explicitly as paths, e.g. "token" or "connect"
        {
            // Serial.println("MATCH for: type");

            globalSettings.set_message_type(value.getString());
        }
        else if (equals_path(&path, "firmware_url", false)) // root elements need to be named explicitly as paths, e.g. "token" or "connect"
        {
            // Serial.println("MATCH for: firmware_url");

            globalSettings.set_firmware_url(value.getString());
        }
        else if (equals_path(&path, "config.status", true))
        {
            // Serial.println("MATCH for: config.status");

            if (!strcmp(path.getKey(), "max_seconds"))
            {
                globalSettings.max_seconds = value.getInt();
            }
            else if (!strcmp(path.getKey(), "refresh_rate_secs"))
            {
                globalSettings.refresh_rate_secs = value.getInt();
            }
        }
        else if (equals_path(&path, "config.status.user_info", true))
        {
            // Serial.println("MATCH for: config.status.user_info");

            if (!strcmp(path.getKey(), "available_time"))
            {
                globalSettings.available_time = value.getInt();
            }
            else if (!strcmp(path.getKey(), "available_type"))
            {
                globalSettings.available_type = value.getInt();
            }
            else if (!strcmp(path.getKey(), "local_seconds_of_day"))
            {
                globalSettings.local_seconds_of_day = value.getInt();
            }
        }
    } // end value

    // Functions we don't care about in this example
    void startDocument()
    {
    }
    void startObject(ElementPath path)
    {
    }
    void endObject(ElementPath path)
    { // End of a object '}'
        // Serial.print("End Object Path: ");
        // print_path(&path);
        // Serial.println();
        if (equals_path(&path, "config.status.friends", false))
        {
            // Serial.println("MATCH for: config.status.friends");
            globalFriendsList.storeActualFriend();
        }
    };
    void startArray(ElementPath path)
    {
    }
    void endArray(ElementPath path)
    {
    }
    void whitespace(char c) {}

    void endDocument() {}
};