// all hardware independent graphics code here
#include "hardware.h"
#include "display.h"
#include "graphics.h"
#include <FreeSans18pt7b.h>

extern FriendsList globalFriendsList;
extern GlobalSettings globalSettings;

// lets start to calculate positions and sizes
const uint16_t R_OUTER = TFT_DIAMETER / 2 - 0;
const uint16_t R_UPPER = R_OUTER - 4;
const uint16_t R_SOLID = R_UPPER - 8;
const uint16_t R_INNER = R_SOLID - 8;

const int16_t TEXT_FRAME_PADDING = 20;
const uint16_t FRAME_WIDTH = 2 * (R_INNER - TEXT_FRAME_PADDING);
const uint16_t SYMBOL_PADDING = 8;
const uint16_t FRAME_HEIGHT = 2 * (R_INNER - SMALL_SYMBOLS_PIXELS - 2 * SYMBOL_PADDING);
const uint16_t FRAME_X = (TFT_DIAMETER - FRAME_WIDTH) / 2;
const uint16_t FRAME_Y = (TFT_DIAMETER - FRAME_HEIGHT) / 2;
const uint16_t TEXT_PADDING_TOP = 2;
const uint16_t TEXT_PADDING_BOTTOM = 2;
const uint16_t TEXT_BORDER_RADIUS = 8;
const uint16_t TEXT_PADDING_LEFT = TEXT_BORDER_RADIUS;
const uint16_t BOX_GAP = 5;    // gap between friend boxes
uint16_t BOX_INNER_HEIGHT = 0; // will be calculated in setup_graphics based on the font height + paddings
uint16_t BOX_HEIGHT = 0;       // will be calculated in setup_graphics based on BOX_INNER_HEIGHT + BOX_GAP
uint16_t FRIENDS_IN_FRAME = 0; // will be calculated in setup_graphics based on FRAME_HEIGHT and BOX_INNER_HEIGHT
GFXcanvas16 canvas(FRAME_WIDTH, FRAME_HEIGHT);
int16_t text_x_offset, text_y_offset;
uint16_t text_width, text_height;

void draw_background()
{
    tft->fillScreen(BACKGROUND_COLOR);
}

void setup_graphics()
{
    // Initialize your setup code here
    setup_display();

    draw_background();
    canvas.setFont(&FreeSans18pt7b);
    canvas.getTextBounds("Wy", 0, 0, &text_x_offset, &text_y_offset, &text_width, &text_height);
    BOX_INNER_HEIGHT = text_height + TEXT_PADDING_TOP + TEXT_PADDING_BOTTOM;
    BOX_HEIGHT = BOX_INNER_HEIGHT + BOX_GAP;
    FRIENDS_IN_FRAME = FRAME_HEIGHT % BOX_HEIGHT ? FRAME_HEIGHT / BOX_HEIGHT + 1 : FRAME_HEIGHT / BOX_HEIGHT; // number of friend boxes that fit into the frame

    canvas.setTextWrap(false); // Disable text wrapping
    Serial.println("Graphics setup:");
    Serial.print("R_OUTER: ");
    Serial.println(R_OUTER);
    Serial.print("R_UPPER: ");
    Serial.println(R_UPPER);
    Serial.print("R_SOLID: ");
    Serial.println(R_SOLID);
    Serial.print("R_INNER: ");
    Serial.println(R_INNER);
    Serial.print("Text bounds width: ");
    Serial.println(text_width);
    Serial.print("Text bounds height: ");
    Serial.println(text_height);
    Serial.print("Text bounds x offset: ");
    Serial.println(text_x_offset);
    Serial.print("Text bounds y offset: ");
    Serial.println(text_y_offset);
    Serial.print("FRAME_X: ");
    Serial.println(FRAME_X);
    Serial.print("FRAME_Y: ");
    Serial.println(FRAME_Y);
    Serial.print("Frame width: ");
    Serial.println(FRAME_WIDTH);
    Serial.print("Frame height: ");
    Serial.println(FRAME_HEIGHT);
    Serial.print("Box height: ");
    Serial.println(BOX_HEIGHT);
    Serial.print("Box inner height: ");
    Serial.println(BOX_INNER_HEIGHT);
    Serial.print("Friends in frame: ");
    Serial.println(FRIENDS_IN_FRAME);
}

void drawQRCode(QRCode *qrcode, int symbol, int color)
{
    int qrSize = qrcode->size;
    int scale = symbol;                                // Scale factor to enlarge the QR code
    int offsetX = (TFT_DIAMETER - qrSize * scale) / 2; // Centering the QR code
    int offsetY = (TFT_DIAMETER - qrSize * scale) / 2; // Centering the QR code

    // Draw the QR code on the display
    // WARNING: The loop variables must be uint8_t to match the qrcode_getModule function signature!!
    for (uint8_t y = 0; y < qrSize; y++)
    {
        for (uint8_t x = 0; x < qrSize; x++)
        {
            if (qrcode_getModule(qrcode, x, y))
            {
                tft->fillRect(offsetX + x * scale, offsetY + y * scale, scale, scale, SCHWARZ);
                Serial.print("**");
            }
            else
            {
                tft->fillRect(offsetX + x * scale, offsetY + y * scale, scale, scale, color);
                Serial.print("  ");
            }
        }
        Serial.println();
    }
}

void draw_symbol(int symbol, int symbol_color, int symbol_bg_color, bool upper)
{
    int16_t symbol_x = (TFT_DIAMETER - SMALL_SYMBOLS_PIXELS) / 2;
    uint16_t symbol_y;
    if (upper)
        symbol_y = TFT_DIAMETER / 2 - R_INNER + SYMBOL_PADDING;
    else
        symbol_y = TFT_DIAMETER / 2 + R_INNER - SYMBOL_PADDING - SMALL_SYMBOLS_PIXELS;
    //
    // tft->fillRect(symbol_x, symbol_y, SMALL_SYMBOLS_PIXELS, SMALL_SYMBOLS_PIXELS, BACKGROUND_COLOR);
    int radius = sqrt(SMALL_SYMBOLS_PIXELS * SMALL_SYMBOLS_PIXELS);
    if (symbol > 0 && symbol < sizeof(small_symbols) / sizeof(small_symbols[0]))
    {
        if (symbol_bg_color > -1)
        {
            Serial.print("radius :");
            Serial.println(radius);
            Serial.print("symbol_bg_color :");
            Serial.println(symbol_bg_color);
            tft->fillCircle(symbol_x + SMALL_SYMBOLS_PIXELS / 2, symbol_y + SMALL_SYMBOLS_PIXELS / 2, radius, symbol_bg_color);
        }
        tft->drawBitmap(symbol_x, symbol_y, small_symbols[symbol], SMALL_SYMBOLS_PIXELS, SMALL_SYMBOLS_PIXELS, symbol_color);
    }
    else
    {
        tft->fillCircle(symbol_x + SMALL_SYMBOLS_PIXELS / 2, symbol_y + SMALL_SYMBOLS_PIXELS / 2, radius, BACKGROUND_COLOR);
    }
}

// #########################################################################
// Draw an arc with a defined thickness
// #########################################################################

// x,y == coords of centre of arc
// start_angle = 0 - 359
// seg_count = number of 3 degree segments to draw (120 => 360 degree arc)
// rx = x axis radius
// yx = y axis radius
// w  = width (thickness) of arc in pixels
// colour = 16 bit colour value
// Note if rx and ry are the same an arc of a circle is drawn

// Found on https://forum.arduino.cc/t/adafruit_gfx-fillarc/397741/3

void fillArc(int x, int y, int start_angle, int seg_count, int rx, int ry, int w, unsigned int colour)
{

    byte seg = 3; // Segments are 3 degrees wide = 120 segments for 360 degrees
    byte inc = 3; // Draw segments every 3 degrees, increase to 6 for segmented ring
    // Draw colour blocks every inc degrees
    for (int i = start_angle; i < start_angle + seg * seg_count; i += inc)
    {
        // Calculate pair of coordinates for segment start
        float sx = cos((i - 90) * DEG2RAD);
        float sy = sin((i - 90) * DEG2RAD);
        uint16_t x0 = sx * (rx - w) + x;
        uint16_t y0 = sy * (ry - w) + y;
        uint16_t x1 = sx * rx + x;
        uint16_t y1 = sy * ry + y;

        // Calculate pair of coordinates for segment end
        float sx2 = cos((i + seg - 90) * DEG2RAD);
        float sy2 = sin((i + seg - 90) * DEG2RAD);
        int x2 = sx2 * (rx - w) + x;
        int y2 = sy2 * (ry - w) + y;
        int x3 = sx2 * rx + x;
        int y3 = sy2 * ry + y;

        tft->fillTriangle(x0, y0, x1, y1, x2, y2, colour);
        tft->fillTriangle(x1, y1, x2, y2, x3, y3, colour);
    }
}

void draw_arc(int start_seconds, int end_seconds, uint16_t color_565)
{
    const int segment_angle = 3; // segment_size = 3 degrees
    // Example usage: draw a full circle arc
    // Remember: A clock has only 12 hours, not 24..
    int start_angle = (start_seconds * 360 / (3600 * 12)) % 360;
    Serial.print("start_angle :");
    Serial.println(start_angle);
    int seg_count = ((end_seconds - start_seconds) * 360 / (3600 * 12)) % 360 / segment_angle;
    Serial.print("seq_count :");
    Serial.println(seg_count);
    if (seg_count < 0)
    {
        seg_count = (360 / segment_angle) + seg_count;
    }
    Serial.print("new seq_count :");
    Serial.println(seg_count);
    fillArc(TFT_DIAMETER / 2, TFT_DIAMETER / 2, start_angle, seg_count, R_UPPER, R_UPPER, 8, color_565);
}

void draw_user_state(int available_type, int available_time, bool always_draw_symbol)
{
    if ((available_time > 0 && available_type > 0 || always_draw_symbol))
    {
        SymbolStyle *available_type_style = globalSymbolStyles.getStyleById(available_type);
        if (available_type_style == nullptr)
        {
            Serial.print("ERROR: No symbol for available_type :");
            Serial.println(available_type);
            return;
        }
        draw_symbol(available_type, available_type_style->symbol_style.fg_color, available_type_style->symbol_style.bg_color, true);

        // TODO : Hier müssen im Offline- Modues noch die korrekten Zeiten berechnet werden

        int actual_seconds = globalSettings.local_seconds_of_day;
        int available_time_end_seconds = actual_seconds + available_time;
        draw_arc(actual_seconds, available_time_end_seconds, available_type_style->symbol_style.bg_color);
        draw_arc(available_time_end_seconds, actual_seconds, BACKGROUND_COLOR);
    }
    else
    {
        draw_symbol(0, 0, -1, true);
    }
}
void draw_version_info()
{
    char version_buffer[MAX_NAME_LENGTH];
    int nr_of_chars = snprintf(version_buffer, MAX_NAME_LENGTH, "%s:%s", BRANCH_NAME_STR, FIRMWARE_VERSION_STR, ORIENTATION_STR);
    if (nr_of_chars < 0)
    {
        Serial.println("FATAL! MAX_NAME_LENGTH is too small to keep version info!");
        return;
    }
    int16_t x1, y1;
    uint16_t w, h;
    tft->getTextBounds(version_buffer, 0, 0, &x1, &y1, &w, &h);
    Serial.print("Version info: ");
    Serial.println(version_buffer);
    Serial.print("Version info width: ");
    Serial.println(w);
    Serial.print("Version info height: ");
    Serial.println(h);
    tft->setTextColor(0x8888);
    Serial.print("Version info x position: ");
    Serial.println((tft->width() - w) / 2);
    Serial.print("Version info y position: ");
    Serial.println(tft->height() - h - 40);
    tft->setCursor((tft->width() - w) / 2, tft->height() - h - 40);
    tft->print(version_buffer);
}
void draw_single_friend(Friend *f, uint16_t box_y)
{
    canvas.setCursor(2 * TEXT_PADDING_LEFT + SMALL_SYMBOLS_PIXELS, box_y + TEXT_PADDING_TOP - text_y_offset);
    if (f->style == nullptr)
    {
        Serial.println("ERROR: Friend style is null!");
        return;
    }

    canvas.fillRoundRect(0, box_y, FRAME_WIDTH - 1, BOX_INNER_HEIGHT, TEXT_BORDER_RADIUS, f->style->text_style.bg_color);
    if (f->symbol > 0 && f->symbol < sizeof(small_symbols) / sizeof(small_symbols[0]))
    {
        int16_t available_time_width = (FRAME_WIDTH * f->available_time) / globalSettings.max_seconds;
        canvas.fillRoundRect(0, box_y, available_time_width - 1, BOX_INNER_HEIGHT, TEXT_BORDER_RADIUS, f->style->symbol_style.bg_color);
        canvas.drawBitmap(TEXT_PADDING_LEFT, box_y + (BOX_INNER_HEIGHT - SMALL_SYMBOLS_PIXELS) / 2, small_symbols[f->symbol], SMALL_SYMBOLS_PIXELS, SMALL_SYMBOLS_PIXELS, f->style->symbol_style.fg_color);
    }
    canvas.setTextColor(f->style->text_style.fg_color);
    canvas.print(f->getName());
}

int16_t pixel_size()
{
    return BOX_HEIGHT * globalFriendsList.size();
}

void transparency(int height)
{
    uint16_t background_red = (BACKGROUND_COLOR >> 11) & 0x1F;
    uint16_t background_green = (BACKGROUND_COLOR >> 5) & 0x3F;
    uint16_t background_blue = BACKGROUND_COLOR & 0x1F;
    for (int i = 0; i < height; i++)
    {
        for (int x = 0; x < FRAME_WIDTH; x++)
        {
            // first do the upper part
            uint16_t pixel_color = canvas.getPixel(x, i);
            uint16_t color_red = (pixel_color >> 11) & 0x1F;
            uint16_t color_green = (pixel_color >> 5) & 0x3F;
            uint16_t color_blue = pixel_color & 0x1F;
            color_red = (color_red * i + background_red * (height - i)) / height;
            color_green = (color_green * i + background_green * (height - i)) / height;
            color_blue = (color_blue * i + background_blue * (height - i)) / height;
            uint16_t blended_color = (color_red << 11) + (color_green << 5) + color_blue;
            canvas.drawPixel(x, i, blended_color);
            // now do the lower part
            pixel_color = canvas.getPixel(x, FRAME_HEIGHT - 1 - i);
            color_red = (pixel_color >> 11) & 0x1F;
            color_green = (pixel_color >> 5) & 0x3F;
            color_blue = pixel_color & 0x1F;
            color_red = (color_red * i + background_red * (height - i)) / height;
            color_green = (color_green * i + background_green * (height - i)) / height;
            color_blue = (color_blue * i + background_blue * (height - i)) / height;
            blended_color = (color_red << 11) + (color_green << 5) + color_blue;
            canvas.drawPixel(x, FRAME_HEIGHT - 1 - i, blended_color);
        }
    }
}

void drawFriendsList(uint16_t pixel_position, bool also_offline_friends)
{
    int16_t negative_y_offset = -(pixel_position % BOX_HEIGHT);
    uint16_t friend_box_index = pixel_position / BOX_HEIGHT;
    // Serial.println("Adding Friends List to display...");
    canvas.fillScreen(BACKGROUND_COLOR);
    for (uint16_t i = 0; i < FRIENDS_IN_FRAME + 1; i++)
    {
        uint16_t current_friend_index = (friend_box_index + i) % globalFriendsList.size();

        Friend f = globalFriendsList[current_friend_index];
        if (also_offline_friends || f.available_time > 0)
        {
            uint16_t box_y = i * BOX_HEIGHT + negative_y_offset;
            draw_single_friend(&f, box_y);
        }
    }
    /*
    int y = 30; // Starting y position
    for (std::vector<Friend>::iterator friendIterator = globalFriendsList.begin(); friendIterator != globalFriendsList.end(); ++friendIterator)
    {
        Friend f = *friendIterator;
        canvas.setCursor(120, y);
        // canvas.setTextColor(f.style->text_style.fg_color);
        canvas.setTextColor(f.style->text_style.fg_color);
        canvas.print(f.getName());
        Serial.println(f.getName());

        canvas.setCursor(140, y);
        canvas.setTextColor(f.style->symbol_style.bg_color);
        canvas.print(String(f.available_time) + "%");
        y += 30; // Move to the next line
        canvas.drawBitmap(40, 60, small_symbols[0], SMALL_SYMBOLS_PIXELS, SMALL_SYMBOLS_PIXELS, f.style->symbol_style.fg_color);
        canvas.drawBitmap(40, 90, small_symbols[1], SMALL_SYMBOLS_PIXELS, SMALL_SYMBOLS_PIXELS, f.style->symbol_style.fg_color);
        canvas.drawBitmap(40, 120, small_symbols[2], SMALL_SYMBOLS_PIXELS, SMALL_SYMBOLS_PIXELS, f.style->symbol_style.fg_color);
        canvas.drawBitmap(40, 150, small_symbols[3], SMALL_SYMBOLS_PIXELS, SMALL_SYMBOLS_PIXELS, f.style->symbol_style.fg_color);
        canvas.drawBitmap(80, 60, small_symbols[4], SMALL_SYMBOLS_PIXELS, SMALL_SYMBOLS_PIXELS, f.style->symbol_style.fg_color);
        canvas.drawBitmap(80, 90, small_symbols[5], SMALL_SYMBOLS_PIXELS, SMALL_SYMBOLS_PIXELS, f.style->symbol_style.fg_color);
        canvas.drawBitmap(80, 120, small_symbols[6], SMALL_SYMBOLS_PIXELS, SMALL_SYMBOLS_PIXELS, f.style->symbol_style.fg_color);

        // canvas.drawBitmap(40, 90, home_outline, HOME_OUTLINE_WIDTH, HOME_OUTLINE_HEIGHT, f.style->symbol_style.fg_color);
        // canvas.drawBitmap(40, 120, coffee_outline, COFFEE_OUTLINE_WIDTH, COFFEE_OUTLINE_HEIGHT, f.style->symbol_style.fg_color);
        // canvas.drawBitmap(40, 150, beer_outline, BEER_OUTLINE_WIDTH, BEER_OUTLINE_HEIGHT, f.style->symbol_style.fg_color);
        // canvas.drawBitmap(80, 60, food_turkey, FOOD_TURKEY_HEIGHT, FOOD_TURKEY_WIDTH, f.style->symbol_style.fg_color);
        // canvas.drawBitmap(80, 90, party_popper, PARTY_POPPER_WIDTH, PARTY_POPPER_HEIGHT, f.style->symbol_style.fg_color);
        // canvas.drawBitmap(80, 120, radiobox_blank, RADIOBOX_BLANK_WIDTH, RADIOBOX_BLANK_HEIGHT, f.style->symbol_style.fg_color);
    }
    */
    transparency(10); // apply transparency effect over first 10 pixels
    tft->drawRGBBitmap(FRAME_X, FRAME_Y, canvas.getBuffer(), FRAME_WIDTH, FRAME_HEIGHT);
    // Serial.println("Friends List done...");
}
