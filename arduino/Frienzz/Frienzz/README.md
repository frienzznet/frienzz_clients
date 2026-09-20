# In Work

video for the pinout: https://youtu.be/y_H7HM0oyoo

Demo code: https://github.com/VolosR/BoatGauges

PIN SETUP: 
#define TFT_MISO 5
#define TFT_MOSI 2
#define TFT_SCLK 15
#define TFT_CS    17 // Chip select control pin
#define TFT_DC    16  // Data Command control pin
#define TFT_RST   4  // Reset pin (could connect to RST pin)


GPIO22 (D22) Rotary SW
GPIO21 (D21) Rotary DT
GPIO19 (D19) Rotary CLK


https://hartmut-waller.info/arduinoblog/beispielprogramm-tft/



Auf https://randomnerdtutorials.com/esp32-tft-touchscreen-display-2-8-ili9341-arduino/
findet sich ein Bild und eine Pin-Belegungstabelle.

Das müsste man mal weiterverfolgen!!!

JSON- Streaming: https://docs.arduino.cc/libraries/json-streaming-parser-2/  
ACHTUNG!! Alte Version 2.0.4 muss genommen werden! - Die aktuelle Version 2.0.5 ist fehlerhaft und erkennt Strukturfehler, wo scheinbar keine sind (bei EndObject()) und bricht daraufhin ab


## Symbole:

Auf https://pictogrammers.com/library/mdi/icon/radiobox-blank/ kann man die Google Material Design Icons suchen und als SVG herunterladen

Die SVGs lädt man dann in Inkscape und
  * formatiert die Seite so, dass sie QUADRATISCH ist! (stundenlange Fehlersuche...)  mit einer durch 8 teilbaren Pixelgröße (entweder kommt die Konvertierwebsite oder die Zeichenroutine selber nicht mit "ungraden" Größen klar...) und passt die Zeichnung so an, dass sie mittig in der 24-Pixel Zeichnung sitzt
  * dann gibt man der Zeichnung selber eine helle Farbe
  * und wählt beim PNG- Export Schwarz als Hintergrund- (BG-) Farbe
  * und stellt bei den PNG- Optionen die Farbpalette auf Gray1
  * dann endlich kann man das PNG exportieren

Die PNGs wandelt man dann auf https://notisrac.github.io/FileToCArray/ zum C- Source um...

  * Code format: Hex
  * Palette mod: 1bit line art (1bit/pixel)
  * Endianess: Little Endian
  * static: yes
  * const: yes
  * unsigned: no
  * Data type: uint8_t
  * PROGMEM: yes

Damit kann man dann konvertieren und das Ergebnis erst in Clipboard und von da aus in den Editor kopieren



Icons in PHP: Home.vue ab Zeile 55
Quelle für die Icon- Bitmaps: https://pictogrammers.com/library/mdi/icon/party-popper/


Eine etwas bessere Experimentierplatine: https://www.amazon.de/ESP32C3-Testplatine-Rotierende-Bildschirm-Bewertungsentwicklung/dp/B0FQ5BL8QT (24 €) oder https://www.amazon.de/LCD-Evaluierungsplatine-Bildschirminteraktion-Encoderschalter-Infrarotsteuerung-USB-Typ-C-Schnittstelle/dp/B0DS1WDX7H (15€)

ESPresiff Doku: https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32c3/esp32-c3-lcdkit/user_guide.html

Waveshare ESP32-C6 Mini Dev Board mit Header (Berrybase WS-26976 8.90€) : https://www.berrybase.de/waveshare-esp32-c6-mini-dev-board-dual-prozessor-wifi-6-bt5-160mhz-4mb-flash-mit-header
Rotary Encoder (Berrybase KY-040 1.60 €) : https://www.berrybase.de/drehregler-rotary-encoder-mit-breakoutboard


ESP mit Touchdisplay;
https://www.displaysmodule.com/sale-45487485-esp32-2424s012c-esp32-tft-lcd-display-ips-viewing-resolution-lightweight-design.html

https://homeding.github.io/boards/esp32c3/jczn-esp32-2424s012.htm

Kabel:

https://de.aliexpress.com/item/1005005371248824.html?dp=Cj0KCQiA7rDMBhCjARIsAGDBuEDnXvo5ttZ8L2xTEs1b0VJpqy3VO5v6WF5PcPOmcueSu_uklQqJwdYaArk4EALw_wcB@218812&isdl=y&aff_fsk=_oD2QuRJ&src=DelupeDETOP&aff_platform=aff_feeds&aff_short_key=_oD2QuRJ&pdp_npi=4%40dis%21EUR%213.78%213.59%21%21%21%21%21%40%2112000052956847933%21afff%21%21%21&gatewayAdapt=glo2deu&cn=218812&cv=467061&af=


## State Machine

Die wurde genommen: https://github.com/cotestatnt/AgileStateMachine


Firmware- Update ??:
https://medium.com/@shikharsinha34920/ota-web-based-esp32-easy-1fc1123938dd

## Build Profile

Nach Tagen des Ausprobierens

Gibt man dem Arduino Maker Workshop Profile im Feld "Build Properties" den folgenden String mit auf den Weg, dann kann man (nach wochenlangem Trial & Error) dem Compile endlich ein paar Defines mit übergeben, um nicht jede Firmware- Variante hardcoded im Source parametrieren zu müssen. Man beachte die komische Schreibweise bei mehr als einem Define, der einfach hinter den ersten geschrieben wird, ohne nochmal compiler.cpp.extra_flags mit angeben zu müssen..:

    compiler.cpp.extra_flags="-DHARDWARE_ID=ESP32_2424S012" "-DORIENTATION=SOUTH_UP"



## Web Flasher

https://www.espboards.dev/tools/program/