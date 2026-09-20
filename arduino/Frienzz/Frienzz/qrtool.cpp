// code to generate and draw QR codes

#include "qrtool.h"

void qr_drawQRCode(const char *qrcode_text, int symbol, int symbol_color)
// Generate a QR code for the given WiFi SSID and password
// according to the WiFi QR code standard
// Returns a QRCode structure
// See: https://stackoverflow.com/a/77774915
{
    // The structure to manage the QR code
    QRCode qrcode;
    int version = 5; // QR code version (1-40) ATTENTION: this Version should be adapted to the length of the text to encode, otherwise the QR code will not be generated correctly. Version 5 can encode up to 106 bytes in byte mode with low error correction, so it should be sufficient for our use case (WiFi credentials).
    // see https://github.com/felixerdy/QRCodeGenerator/tree/main#data-capacities for more details on the capacities of different QR code versions and error correction levels.
    // The buffer to store the QR code modules (black and white squares)
    uint8_t qrcodeBytes[qrcode_getBufferSize(version)];

    // qrcode_initText(&qrcode, qrcodeBytes, version, ECC_LOW, qrcode_text);
    qrcode_initBytes(&qrcode, qrcodeBytes, version, ECC_LOW, (uint8_t *)qrcode_text, strlen(qrcode_text));
    drawQRCode(&qrcode, symbol, symbol_color);
}
