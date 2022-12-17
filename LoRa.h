#ifndef LoRa_h
#define LoRa_h

// On the LoRa board, solder jumper wires between:
// - IRQ & E
// - CS & D
// - RST & C

// This device's address.  Must be unique amongst all friends.
#define DEFAULT_DEVICE_ADDRESS 1

#include <SPI.h>
#include <RH_RF95.h>

#define LORA_IRQ_PIN 5 // "E" on the LoRa board
#define LORA_CS_PIN 6 // "D" on the LoRa board
#define LORA_RST_PIN 9 // "C" on the LoRa board


RH_RF95 rf95(LORA_CS_PIN, LORA_IRQ_PIN);

class LoRa {

private:
    

public:
    void setup() {
        Serial.println(F("Initializing LoRa"));
        pinMode(LORA_RST_PIN, OUTPUT);
        digitalWrite(LORA_RST_PIN, HIGH);

        // manual reset
        digitalWrite(LORA_RST_PIN, LOW);
        delay(10);
        digitalWrite(LORA_RST_PIN, HIGH);
        delay(10);

        if (!rf95.init()) {
          Serial.println(F("WARNING: LoRa init Failed!"));
          Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info.");
           return;
        }

        // TODO: store and retrieve user-configured device ID from EEPROM
        rf95.setThisAddress(DEFAULT_DEVICE_ADDRESS);
        rf95.setHeaderFrom(DEFAULT_DEVICE_ADDRESS);

        // Note: Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on.
        // The default transmitter power is 13dBm, using PA_BOOST.
        // If we are using an RFM95 module which uses the PA_BOOST transmitter pin, then 
        // we can set transmitter powers from 5 to 23 dBm using this command:
        // rf95.setTxPower(23, false);

        if (!rf95.setFrequency(915.0)) {
          Serial.println(F("WARNING: setFrequency Failed!"));
         return;
        }
        
        Serial.println(F("LoRa Initialized!"));
    }

    void loop() {
      // Check for new messages
      if (rf95.available()) {
        // Should be a message for us now   
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);
        if (rf95.recv(buf, &len)) {
          Serial.print("LoRa received from ");
          Serial.print(rf95.headerFrom());
          Serial.print(" to ");
          Serial.print(rf95.headerTo());
          Serial.print(": ");
          Serial.println((char*)buf);
        } else {
          Serial.println("failed to parse LoRa message.");
        }
      }
    }

    void requestLocationFromDevice(uint8_t targetDeviceAddress) {
      rf95.setHeaderTo(targetDeviceAddress);
      uint8_t data[] = "hvar ertu?";
      rf95.send(data, sizeof(data));
    }
};

#endif