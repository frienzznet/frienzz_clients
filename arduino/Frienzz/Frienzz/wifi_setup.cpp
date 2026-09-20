// do the wifi related stuff here

#include <esp_mac.h>
#include "wifi_setup.h"

char qrcode_wifi_string[QR_CODE_AP_STRING_SIZE];

void setup_wifi()
// Generate a QR code for the given WiFi SSID and password
// according to the WiFi QR code standard
// Returns a QRCode structure
{

    char ssid[22];
    snprintf(ssid, 22, "frienzz-%llX", ESP.getEfuseMac());
    char *password = "frienzzfrienzz";
    // See: https://stackoverflow.com/a/77774915
    snprintf(qrcode_wifi_string, QR_CODE_AP_STRING_SIZE, "WIFI:T:WPA;S:%s;P:%s;;", ssid, password);
    // first parameter is name of access point, second is the password
    WiFiManager wifiManager;

    // reset settings - wipe stored credentials for testing
    // these are stored by the esp library
    // wifiManager.resetSettings();

    // TDOO: create random password
    // TODO: draw a QR code on the display for easy connection
    // wifiManager.setAPCallback(configModeCallback);
    // TODO: save the user settings with the wifi manager
    wifiManager.setAPCallback(show_AP_QRCode);
    wifiManager.autoConnect(ssid, password);
}

void saveUserSettings()
// TODO: implement saving user settings
//  save the device id
{
}

void loop_wifi()
{
}