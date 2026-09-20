#include "display.h"
#include "wifi_setup.h"
#include "qrtool.h"
#include "graphics.h"
#include "uidevice.h"
#include "uirotary.h"
#include "uitouch.h"
#include "webio.h"
#include "statemachine.h"
#include "configfs.h"

extern char token[MAX_VALUE_LENGTH];
extern char connect[MAX_VALUE_LENGTH];
#if defined(HAS_ROTARY_ENCODER)
UIDevice *uidevice = new UIRotary(12 * 60 * 60, 20, 1, 5); // ticksPerFullTurn, numberOfInnerGears, numberOfOuterGears
#endif
#if defined(HAS_TOUCHSCREEN)
UIDevice *uidevice = new UITouch();
#endif
extern FriendsList globalFriendsList;
extern GlobalSettings globalSettings; // global settings object
void setup()
{

  Serial.begin(115200);
  while (!Serial)
  {
    delay(5);
  }
  create_default_symbol_styles();
  configfs_init();
  setup_graphics();
  uidevice->setup();
  // wifi setup comes last to have display ready for QR code
  setup_wifi();
  setup_state_machine();
}

void show_AP_QRCode(WiFiManager *myWiFiManager)
{
  Serial.println("Welcome Screen");
  Serial.println(qrcode_wifi_string);
  unsigned long start = millis();
  #if defined(DEBUG)
    qr_drawQRCode("http://192.168.1.185:8081/welcome.html", 4, WEISS);
  #else
    qr_drawQRCode("https://frienzz.net/welcome.html", 4, WEISS);
  #endif
  while (!uidevice->button_release_event() && millis() - start < 10000) // wait for button press or 10 seconds
    ;
  Serial.println("Entered AP Mode");
  Serial.println(qrcode_wifi_string);

  qr_drawQRCode(qrcode_wifi_string, 4, WEISS);
}

void loop()
{

  loop_state_maschine();
}