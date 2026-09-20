// all hardware independent graphics code here

#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "display.h"
#include "qrtool.h"
#include "dataobjects.h"
#include "symbol-beer-outline.h"
#include "symbol-coffee-outline.h"
#include "symbol-food-turkey.h"
#include "symbol-home-outline.h"
#include "symbol-party-popper.h"
#include "symbol-phone-outline.h"
#include "symbol-radiobox-blank.h"

#define SMALL_SYMBOLS_PIXELS 24

#define DEG2RAD 0.0174532925

static const uint8_t *small_symbols[] = {
    radiobox_blank,
    phone_outline,
    home_outline,
    coffee_outline,
    beer_outline,
    food_turkey,
    party_popper};

void draw_background();
void setup_graphics();
int16_t pixel_size();
void draw_arc(int start_seconds, int end_seconds, uint16_t color_565);
void draw_user_state(int available_type, int available_time, bool always_draw_symbol);
void draw_version_info();
void draw_symbol(int symbol, int symbol_color, bool upper);
void draw_single_friend(Friend *f, int16_t box_y);
void drawQRCode(QRCode *qrcode, int symbol, int symbol_color);
void drawFriendsList(uint16_t pixel_position, bool also_offline_friends);
#endif