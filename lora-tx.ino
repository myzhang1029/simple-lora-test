#include <SPI.h>
// From "ESP8266 and ESP32 OLED driver for SSD1306 displays"
// https://github.com/ThingPulse/esp8266-oled-ssd1306
#include "SSD1306Wire.h"
// https://github.com/sandeepmistry/arduino-LoRa
#include "LoRa.h"
#include "opensans.h"


#define SCK 5
#define MISO 19
#define MOSI 27
#define NSS 18
#define RST 14
#define DIO0 26

#define BAND 433E6
#define FREQ 438.525E6
//#define RX
#undef RX

#define Display SSD1306Wire
#define DISPLAY_ADDRESS 0x3C
#define DISPLAY_SDA 21
#define DISPLAY_SCL 22
Display display(DISPLAY_ADDRESS, DISPLAY_SDA, DISPLAY_SCL);

void drawBasicInfo() {
    display.drawString(0, 0, "https://WCSNG.ucsd.edu");
    display.drawString(0, 10, "UC Property Do Not Move");
    display.drawString(0, 20, "Maiyun MAZ005@ucsd.edu");
}

void setup() {
    Serial.begin(9600);

#ifdef RX
    Serial.println(F("LoRa Receiver"));
#else
    Serial.println(F("LoRa Transmitter"));
#endif

    SPI.begin(SCK, MISO, MOSI, NSS);
    LoRa.setPins(NSS, RST, DIO0);
    
    if (!LoRa.begin(BAND)) {
        Serial.println(F("Failed to start LoRa"));
        while (1);
    }
    LoRa.setFrequency(FREQ);
    LoRa.setSpreadingFactor(8);
    LoRa.setSignalBandwidth(125000);
#ifndef RX
    LoRa.setTxPower(10);
#endif

    display.init();
    display.setFont(Open_Sans_Light_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    drawBasicInfo();
    display.display();

    delay(2000);
}

#ifdef RX
void loop() {
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        int rssi, snr;
        char buffer[256], *bp = buffer;
        Serial.print(F("Received \""));
        while (LoRa.available())
            Serial.print((*bp++ = (char) LoRa.read()));
        *bp = '\0';
        rssi = LoRa.packetRssi();
        snr = LoRa.packetSnr();
        Serial.print(F("\" RSSI="));
        Serial.print(rssi);
        Serial.print(F("dBm SNR="));
        Serial.print(snr);
        Serial.println(F("dB"));
        display.clear();
        drawBasicInfo();
        display.drawString(5, 40, String(snr) + "dBSNR @ " + String(rssi) + "dBm");
        display.drawString(5, 50, buffer);
        display.display();
    }
}
#else
unsigned int counter = 0;
void loop() {
    char buffer[4];
    Serial.print(F("Sending packet "));
    snprintf(buffer, sizeof(buffer), "%03x", counter);
    Serial.println(buffer);
    //Send LoRa packet to receiver
    LoRa.beginPacket();
    LoRa.print(F("AK6DS "));
    LoRa.print(buffer);
    LoRa.endPacket();
    ++counter;
    counter &= 0xFFF;
    display.clear();
    drawBasicInfo();
    display.drawString(5, 40, "Sent packet " + String(buffer));
    display.display();
    delay(2000);
}
#endif
