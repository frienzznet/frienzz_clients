// code to generate and draw QR codes

#ifndef QRTOOL_H
#define QRTOOL_H

#include "Arduino.h" // needed for Serial prints
#include "QRCodeGenerator.h"
#include "graphics.h"

void qr_drawQRCode(const char *qrcode_text, int symbol, int symbol_color);

#endif