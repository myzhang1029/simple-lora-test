#include <SPI.h>
#include "LoRa.h"

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

void setup() {
    Serial.begin(9600);

    Serial.println("LoRa Transmitter");

    SPI.begin(SCK, MISO, MOSI, NSS);
    LoRa.setPins(NSS, RST, DIO0);
    
    if (!LoRa.begin(BAND)) {
        Serial.println("Failed to start LoRa");
        while (1);
    }
    LoRa.setFrequency(FREQ);
    LoRa.setSpreadingFactor(8);
    LoRa.setSignalBandwidth(125000);
#ifndef RX
    LoRa.setTxPower(10);
#endif
    delay(2000);
}

#ifdef RX
void loop() {
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        Serial.print("Received ");
        while (LoRa.available()) {
            Serial.print((char)LoRa.read());
        }
        Serial.println();
    }
}
#else
unsigned int counter = 0;
void loop() {
    char buffer[4];
    Serial.print("Sending packet ");
    snprintf(buffer, sizeof(buffer), "%03d", counter);
    Serial.println(buffer);
    //Send LoRa packet to receiver
    LoRa.beginPacket();
    LoRa.print("AK6DS ");
    LoRa.print(buffer);
    LoRa.endPacket();
    ++counter;
    delay(2000);
}
#endif
