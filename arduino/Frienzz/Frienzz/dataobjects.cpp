// the different data objects used in Frienzz
#include "dataobjects.h"

StyleSetting defaultStyleSetting = StyleSetting();

FriendsList globalFriendsList; // global list of friends
                               // std::vector<Friend>::iterator friendIterator; // global iterator for friends list

// as hardcoded default styles for symbols

int defaultStyleValues[7][4] = {
    // text_style.fg_color , text_style.bg_color , style->symbol_style.fg_color , symbol_style.bg_color
    {0x0000, 0x70efb0, 0x0FF, 0x1E88E5},  // Symbol 0
    {0x0000, 0x70efb0, 0x0000, 0x1E88E5}, // Symbol 1
    {0x0000, 0x70efb0, 0x0000, 0x43A047}, // Symbol 2
    {0x0000, 0x70efb0, 0x0000, 0xFB8C00}, // Symbol 3
    {0x0000, 0x70efb0, 0x0000, 0xFDD835}, // Symbol 4
    {0x0000, 0x70efb0, 0x0000, 0xD81B60}, // Symbol 5
    {0x0000, 0x70efb0, 0x0000, 0xE53935}, // Symbol 6

};
SymbolStyles globalSymbolStyles; // global list of symbol styles

void create_default_symbol_styles()
{

    for (int i = 0; i < sizeof(defaultStyleValues) / sizeof(*defaultStyleValues); i++)
    {
        SymbolStyle newStyle;
        newStyle.id = i;
        int rgb888 = defaultStyleValues[i][0];
        uint16_t rgb565 = (((rgb888 & 0xf80000) >> 8) + ((rgb888 & 0xfc00) >> 5) + ((rgb888 & 0xf8) >> 3));
        newStyle.text_style.fg_color = rgb565;
        rgb888 = defaultStyleValues[i][1];
        rgb565 = (((rgb888 & 0xf80000) >> 8) + ((rgb888 & 0xfc00) >> 5) + ((rgb888 & 0xf8) >> 3));
        newStyle.text_style.bg_color = rgb565;
        rgb888 = defaultStyleValues[i][2];
        rgb565 = (((rgb888 & 0xf80000) >> 8) + ((rgb888 & 0xfc00) >> 5) + ((rgb888 & 0xf8) >> 3));
        newStyle.symbol_style.fg_color = rgb565;
        rgb888 = defaultStyleValues[i][3];
        rgb565 = (((rgb888 & 0xf80000) >> 8) + ((rgb888 & 0xfc00) >> 5) + ((rgb888 & 0xf8) >> 3));
        newStyle.symbol_style.bg_color = rgb565;
        globalSymbolStyles.addStyle(newStyle);
        Serial.print("Created default symbol style with id ");
        Serial.println(newStyle.id);
    }
}

GlobalSettings globalSettings; // global settings object