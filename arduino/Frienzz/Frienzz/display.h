#ifndef DISPLAY_H
#define DISPLAY_H
// all hardware related graphics code goes here to decouple from hardware independent code

#include "hardware.h"

// Farben
#define SCHWARZ 0x0000
#define WEISS 0xFFFF
#define BLAU 0x001F
#define ROT 0xF800
#define GRUEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define GELB 0xFFE0
#define BRAUN 0x9A60
#define GRAU 0x7BEF
#define GRUENGELB 0xB7E0
#define DUNKELCYAN 0x03EF
#define ORANGE 0xFDA0
#define PINK 0xFE19
#define BORDEAUX 0xA000
#define HELLBLAU 0x867D
#define VIOLETT 0x915C
#define BACKGROUND_COLOR 0xC618
#define GOLD 0xFEA0

DISPLAY_CONSTRUCTOR_TYPE *setup_display();
void loop_display();

#endif