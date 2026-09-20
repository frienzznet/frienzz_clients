// load and save configuration using ArduinoJson and SPIFFS

// Based on https://github.com/tzapu/WiFiManager/blob/master/examples/Parameters/SPIFFS/AutoConnectWithFSParameters/AutoConnectWithFSParameters.ino

#include <FS.h> //this needs to be first, or it all crashes and burns...

#ifdef ESP32
#include <SPIFFS.h>
#endif

#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson

#include "configfs.h"

/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */
#define FORMAT_SPIFFS_IF_FAILED true

extern GlobalSettings globalSettings; // global settings object

void configfs_init()
{
    // configfs_init must be called at an early stage in the setup() function, before any attempt to read or write configuration parameters, to ensure that the SPIFFS file system is properly initialized and ready for use.

    // clean FS, for testing
    // SPIFFS.format();

    // read configuration from FS json
    Serial.println("mounting FS...");

    if (SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
    {
        Serial.println("mounted file system");
        if (SPIFFS.exists("/frienzz.json"))
        {
            // file exists, reading and loading
            Serial.println("reading config file");
            File configFile = SPIFFS.open("/frienzz.json", "r");
            if (configFile)
            {
                Serial.println("opened config file");
                size_t size = configFile.size();
                // Allocate a buffer to store contents of the file.
                std::unique_ptr<char[]> buf(new char[size]);

                configFile.readBytes(buf.get(), size);

#if defined(ARDUINOJSON_VERSION_MAJOR) && ARDUINOJSON_VERSION_MAJOR >= 6
                DynamicJsonDocument json(1024);
                auto deserializeError = deserializeJson(json, buf.get());
                serializeJson(json, Serial);
                if (!deserializeError)
                {
                    globalSettings.set_token(json["token"]);
                    globalSettings.watchface_id = json["watchface_id"];
                    globalSettings.watchface_param_id = json["watchface_param_id"];
                    Serial.println("\nparsed json");
                }
                else
                {
                    Serial.println("failed to load json config");
                }

#else
                DynamicJsonBuffer jsonBuffer;
                JsonObject &json = jsonBuffer.parseObject(buf.get());
                json.printTo(Serial);
                if (json.success())
                {
                    globalSettings.set_token(json["token"]);
                    globalSettings.watchface_id = json["watchface_id"];
                    globalSettings.watchface_param_id = json["watchface_param_id"];
                    Serial.println("\nparsed json");
                }
                else
                {
                    Serial.println("failed to load json config");
                }
#endif
                configFile.close();
            }
        }
        else
        {
            Serial.println("no config file found");
        }
    }
    else
    {
        Serial.println("failed to mount FS ");
    }
    // end read
}

void configfs_save()
{
    // save the custom parameters to FS

    Serial.println("saving config");
#if defined(ARDUINOJSON_VERSION_MAJOR) && ARDUINOJSON_VERSION_MAJOR >= 6
    DynamicJsonDocument json(1024);
#else
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();
#endif

    json["token"] = globalSettings.get_token();
    json["watchface_id"] = globalSettings.watchface_id;
    json["watchface_param_id"] = globalSettings.watchface_param_id;

    File configFile = SPIFFS.open("/frienzz.json", "w");
    if (configFile)
    {
        Serial.println("opened config file for writing");
#if defined(ARDUINOJSON_VERSION_MAJOR) && ARDUINOJSON_VERSION_MAJOR >= 6
        serializeJson(json, Serial);
        serializeJson(json, configFile);
#else
        json.printTo(Serial);
        json.printTo(configFile);
#endif
        configFile.close();
    }
    else
    {
        Serial.println("failed to open config file for writing");
    }

    // end save
}
