//all hardware independent graphics code here

#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#define QR_CODE_AP_STRING_SIZE 99
extern char qrcode_wifi_string[QR_CODE_AP_STRING_SIZE];
void setup_wifi();
void loop_wifi();
void show_AP_QRCode (WiFiManager *myWiFiManager);
#endif