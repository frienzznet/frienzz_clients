/*
 * taken from https://github.com/mrcodetastic/json-streaming-parser2/blob/master/examples/HTTPS_StreamParser_WeatherESP32/HTTPS_StreamParser_WeatherESP32.ino
 *
 * The use of SSL is by far the biggest consumer of memory, with over 20kB of SRAM consumed by this alone.
 */

// https code from https://randomnerdtutorials.com/esp32-https-requests/

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include <WiFiClientSecure.h>

#include "webiohandler.h"        // Custom JSON document handler
#include <ArduinoStreamParser.h> // <==== THE JSON Streaming Parser - Arduino STREAM WRAPPER
#include "webio.h"

extern FriendsList globalFriendsList;
unsigned long webio_update_interval_micros = 0;
// suppress a webio_loop to save time, if a call just just made insteandly before
bool suppress_next_webio_loop = false;

char token[MAX_VALUE_LENGTH] = "";

#if defined(DEBUG)
const char *server = "192.168.1.185"; // Server URL
const int port = 8000;

WiFiClient client;

#else
const char *server = "frienzz.net"; // Server URL
const int port = 443;
//  const char *server = "www.howsmyssl.com"; // Server URL

const char *test_root_ca =
    "-----BEGIN CERTIFICATE-----\n"
    "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\n"
    "-----END CERTIFICATE-----\n";

// You can use x.509 client certificates if you want
// const char* test_client_key = "";   //to verify the client
// const char* test_client_cert = "";  //to verify the client

WiFiClientSecure client;

#endif

HTTPClient http;
ArudinoStreamParser parser;  // <---------- Declare stream parser
WebIOHandler custom_handler; // <---------- Declare stream handler
// static HttpsOTAStatus_t otastatus;

char json_buffer[JSON_BUFFER_SIZE];

void printHeapFreeToSerial()
{
    Serial.print("Heap free: ");
    Serial.println(String(ESP.getFreeHeap(), DEC));
}

bool send_json_rpc(char *json_content)
{

    parser.setHandler(&custom_handler); // <--------- Link to customer listener (parser to be honest)

    Serial.println("Setup JSON Streaming Parser.");
    printHeapFreeToSerial();
#if !defined(DEBUG)
    // Use WiFiClientSecure class to create TLS connection

    // client.setCACert(test_root_ca);
    client.setInsecure(); // skip verification for testing only
// client.setCertificate(test_client_cert); // for client verification
// client.setPrivateKey(test_client_key);	// for client verification
#endif
    Serial.print("\nStarting connection to server...");
    Serial.print(server);
    Serial.print(":");
    Serial.println(port);
    if (!client.connect(server, port))
    {
        Serial.println("Connection failed!");
        return false;
    }

    Serial.println("Connected to server!");
    // Make a HTTP request:
    // client.println("GET https://samples.openweathermap.org/data/2.5/forecast?q=London,us&appid=439d4b804bc8187953eb36d2a8c26a02 HTTP/1.0");
    // client.println("Host: samples.openweathermap.org");

    // creating a POST message https://stackoverflow.com/a/32537358
    client.println("POST /xhr.php HTTP/1.1");
    // client.println("Host: 92.168.1.185");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println("Accept: */*");
    client.print("User-Agent: ");
    client.println(DEVICE);
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(strlen(json_content));
    client.println();
    client.print(json_content);

    // if there are incoming bytes available
    // from the server, read them and print them:
    int detect_empty_lines = 0;
    while (client.connected() || client.available())
    {                               // loop while connected or data available
        if (detect_empty_lines < 2) // still in headers
        {
            while (client.available())
            {
                char c = client.read();
                Serial.write(c);
                if (c == '\r')
                {
                    continue; // skip CR characters
                }
                if (c == '\n')
                {
                    detect_empty_lines++;
                    if (detect_empty_lines == 2)
                    {
                        Serial.println("headers received");
                        break; // end of headers}
                    }
                }
                else
                {
                    detect_empty_lines = 0; // reset counter
                }
            }
        }
        else
        {
            globalFriendsList.clear(); // clear previous data
            while (client.available())
            {
                char c = client.read();
                Serial.write(c);
                {
                    parser.parse(c); /* code */
                    // if (parser.hasParseError())
                    // {
                    //     Serial.println(parser.getErrorMessage());
                    // }
                }
            }
            delay(1); // allow the cpu to rest
        }
    }

    client.stop();

    Serial.println("Done.");
    printHeapFreeToSerial();

    return true;
}

bool send_serial_json_rpc(char *json_content)
{
    parser.setHandler(&custom_handler); // <--------- Link to customer listener (parser to be honest)

    if (json_content != NULL)
    {
        Serial.println(json_content);
        Serial.print(0x04); // send EOT character to signal end of message
    }

    // if there are incoming bytes available
    // from the serial port, read them and print them:
    if (Serial.available())
    {
        globalFriendsList.clear(); // clear previous data
        while (Serial.available())
        {
            char c = Serial.read();
            {
                parser.parse(c); /* code */
                // if (parser.hasParseError())
                // {
                //     Serial.println(parser.getErrorMessage());
                // }
            }
        }
        delay(1); // allow the cpu to rest
        return true;
    }
    else
    {
        return false;
    }
}

void check_for_firmware_update()
{
    const char *firmware_url = globalSettings.get_firmware_url();
    if (firmware_url[0] != 0)
    {
        Serial.println("Firmware update requested.");
        Serial.print("Firmware URL: ");
        Serial.println(firmware_url);
        Serial.println("Starting OTA update...");
// Start the OTA update
#if !defined(DEBUG)
        // client.setCACert(test_root_ca);
        client.setInsecure(); // skip verification for testing only
                              // client.setCertificate(test_client_cert); // for client verification
                              // client.setPrivateKey(test_client_key);	// for client verification
#endif
        Serial.print("Connecting to server: ");
        Serial.println(firmware_url);
        if (http.begin(client, firmware_url))
        {
            int httpCode = http.GET();
            if (httpCode == HTTP_CODE_OK)
            {
                Serial.println("Updating firmware...");
                // Start the update process
                int updateSize = http.getSize();
                Serial.print("Firmware size: ");
                Serial.println(updateSize);
                if (updateSize <= 0)
                {
                    Serial.println("Invalid content length");
                    return;
                }
                if (Update.begin(updateSize))
                {
                    Stream &updateStream = http.getStream();
                    size_t written = Update.writeStream(updateStream);
                    if (written == updateSize)
                    {
                        if (Update.end())
                        {
                            Serial.println("Update erfolgreich! Starte neu...");
                            ESP.restart();
                        }
                        else
                        {
                            Serial.printf("Fehler bei Update. End(): %s\n", Update.getError());
                        }
                    }
                    else
                    {
                        Serial.printf("Fehler: Es wurden nur %d von %d Bytes geschrieben\n", written, updateSize);
                    }
                }
                else
                {
                    Serial.println("Unable to begin firmware update");
                }
            }
            else
            {
                Serial.print("Firmware update failed. Server returned HTTP code: ");
                Serial.println(httpCode);
            }

            http.end();
        }
        else
        {
            Serial.println("Unable to connect to server");
        }
        globalSettings.set_firmware_url(""); // clear the firmware url to avoid repeated updates
    }
}

bool loop_webio(bool force_update)
{
    if (!force_update && !intervalElapsed(&webio_update_interval_micros, 1UL * 60UL * 1000000UL)) // 1 minutes
    {
        send_serial_json_rpc(NULL);
        check_for_firmware_update();
        return false;
    }
    if (suppress_next_webio_loop)
    {
        suppress_next_webio_loop = false;
        send_serial_json_rpc(NULL);
        check_for_firmware_update();
        return false;
    }
    int nr_of_chars = snprintf(json_buffer, JSON_BUFFER_SIZE, "{\"type\":\"home_status\",\"config\":{},\"device\": \"%s:%s:%s:%s\", \"token\":\"%s\"}", HARDWARE_ID_STR, BRANCH_NAME_STR, FIRMWARE_VERSION_STR, ORIENTATION_STR, globalSettings.get_token());
    if (nr_of_chars < 0)
    {
        Serial.println("FATAL! JSON_BUFFER_SIZE is too small to keep JSON message!");
        return false;
    }
    Serial.print("Sending JSON-RPC: ");
    Serial.println(json_buffer);
    bool success = send_json_rpc(json_buffer);
    send_serial_json_rpc(json_buffer);
    check_for_firmware_update();

    for (std::vector<Friend>::iterator friendIterator = globalFriendsList.begin(); friendIterator != globalFriendsList.end(); ++friendIterator)
    {
        Friend f = *friendIterator;
        Serial.print("Friend Name: ");
        Serial.print(f.getName());
        Serial.print(", available_time: ");
        Serial.println(f.available_time);
    }

    Serial.println("Done.");
    printHeapFreeToSerial();

    return true;
}

bool webio_jsonrpc_send_status(int available_type, int available_time)
{
    int nr_of_chars = snprintf(json_buffer, JSON_BUFFER_SIZE,
                               "{\"type\":\"user_set_available_state\",\"config\":{\"available_type\":%d,\"available_time\":%d},\"device\": 1,\"token\":\"%s\"}",
                               available_type, available_time, globalSettings.get_token());
    if (nr_of_chars < 0)
    {
        Serial.println("FATAL! JSON_BUFFER_SIZE is too small to keep JSON message!");
        return false;
    }
    bool success = send_json_rpc(json_buffer);
    send_serial_json_rpc(json_buffer);
    suppress_next_webio_loop = true;
    /*
    nr_of_chars = snprintf(json_buffer, JSON_BUFFER_SIZE,
                           "{\"type\":\"user_set_available_time\",\"config\":{\"available_time\":%d},\"device\": 1,\"token\":\"%s\"}",
                           available_time, globalSettings.get_token());
    if (nr_of_chars < 0)
    {
        Serial.println("FATAL! JSON_BUFFER_SIZE is too small to keep JSON message!");
        return false;
    }
    success = send_json_rpc(json_buffer);
    */
    return true;
}