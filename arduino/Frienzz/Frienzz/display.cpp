// all hardware related graphics code goes here to decouple from hardware independent code

#include "display.h"

DISPLAY_CONSTRUCTOR_TYPE *tft;

DISPLAY_CONSTRUCTOR_TYPE *
setup_display()
{

  DISPLAY_INIT_SEQUENCE

  Serial.println("TFT object created");

  Serial.println("Bildschirm: " + String(tft->height()) + " x " + String(tft->width()));

  Serial.print("The board specific (VSPI-) SPI Pins : ");
  Serial.print("MOSI: ");
  Serial.println(MOSI);
  Serial.print("MISO: ");
  Serial.println(MISO);
  Serial.print("SCK: ");
  Serial.println(SCK);
  Serial.print("SS: ");
  Serial.println(SS);

#if defined(TFT_BL)
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH); // Backlight on
#endif                        // end TFT_BL

  // Rotation anpassen
  tft->setRotation(TFT_ROTATION);
  Serial.println("TFT rotation set");
  tft->fillScreen(BACKGROUND_COLOR);
  Serial.println("Background drawn");
  return tft;
}

void loop_display()
{

  // schwarzer Hintergrund
  tft->fillScreen(SCHWARZ);

  // interne Textdarstellung
  tft->setTextSize(3);
  tft->setCursor(30, 50);
  tft->setTextColor(BLAU);
  tft->print("Text");
  delay(500);

  tft->setTextSize(4);
  tft->setCursor(30, 80);
  tft->setTextColor(GRUEN);
  tft->print("Text");
  delay(2000);

  // zufällige Pixel
  tft->fillScreen(SCHWARZ);
  for (int i = 0; i < 700; i++)
  {
    int PixelX = random(1, tft->width());
    int PixelY = random(1, tft->height());
    tft->drawPixel(PixelX, PixelY, tft->color565(random(255), random(255), random(255)));
    // delay(5);
  }
  // delay(2000);

  // Kreise vom Mittelpunkt zeichnen
  tft->fillScreen(SCHWARZ);
  for (int i = 1; i < tft->width() / 2; i += 5)
  {
    tft->drawCircle(tft->width() / 2, tft->height() / 2, tft->width() / 2 - i, tft->color565(random(255), random(255), random(255)));
    // delay(50);
  }
  // delay(2000);

  for (int i = 1; i < tft->width() / 2; i += 5)
  {
    tft->drawCircle(tft->width() / 2, tft->height() / 2, tft->width() / 2 - i, SCHWARZ);
    // delay(50);
  }
  // delay(2000);

  // Kreise vom Mittelpunkt zeichnen
  tft->fillScreen(SCHWARZ);
  for (int i = 1; i < tft->width() / 2; i += 10)
  {
    tft->fillCircle(tft->width() / 2, tft->height() / 2, tft->width() / 2 - i, tft->color565(random(255), random(255), random(255)));
    // delay(50);
  }
  // delay(2000);

  /*
    alle 30° Linie vom Mittelpunkt zeichnen
    120 = Radius des TFTs (240/2)
    90 = PI/4 (Viertelkreis)
    DEG_TO_RAD (0.0174532925) -> Winkel in Bogenmaß umrechnen
    es ensteht ein Dreieck mit einem Rechten Winkel
    -> Winkelfunktionen sin/cos anwenden
  */
  tft->fillScreen(SCHWARZ);
  for (int i = 0; i < 360; i += 30)
  {
    float PosX = cos((i - 90) * DEG_TO_RAD) * 120;
    float PosY = sin((i - 90) * DEG_TO_RAD) * 120;
    int PunktX = PosX + 120;
    int PunktY = PosY + 120;
    tft->drawLine(120, 120, PosX + 120, PosY + 120, WEISS);

    // delay(2000);
  }
}