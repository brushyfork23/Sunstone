// Sunstone
// Handheld Black Rock City Wayfinder

// Upload this file to the Adafruit ESP32-S2 TFT Feather - http://www.adafruit.com/products/5300
// Connect to the Adafruit GPS FeatherWing - https://www.adafruit.com/products/3133

// Use Arduino Board Manager to install the required libraries (and all dependencies):
// Adafruit LC709203F - Battery Monitor
// Adafruit ST7735 and ST7789 Library - TFT Display
// Adafruit GPS Library - GPS
// Chrono - Timing
// Bounce2 - Buttons

// Manually install this library:
// http://www.airspayce.com/mikem/arduino/RadioHead/ - LoRa

// Default list of friends' device addresses
uint8_t friendAddresses[] = {2};

// Timing
#include <LightChrono.h>

// Buttons and Switches
#include <Bounce2.h>
#define PIN_POWER_SWITCH 16 // A2
#define PIN_GHOST_MODE_SWITCH 13
#define PIN_DEVELOPER_SWITCH 15 // A3
#define PIN_PREVIOUS_BUTTON 12
#define PIN_NEXT_BUTTON 11
#define PIN_SELECT_BUTTON 10
Bounce powerSwitch = Bounce();
Bounce ghostModeSwitch = Bounce();
Bounce developerSwitch = Bounce();
Bounce2::Button previousBtn = Bounce2::Button();
Bounce2::Button nextBtn = Bounce2::Button();
Bounce2::Button selectBtn = Bounce2::Button();

// Display
#include "Display.h"
Display display;

// GPS
#include "GPS.h"
GPS gps;

// LoRa
#include "LoRa.h"
LoRa lora;

// GPS display refresh timer
const long displayUpdateGPSInterval = 1000; // milliseconds between refreshing displayed GPS location and clock
LightChrono gpsDisplayUpdateTimer;

// LoRa friend ping timer
const long loraFindFriendInterval = 60000; // milliseconds between pinging a friend for their location
LightChrono loraFindFriendTimer;

void setup() {
    delay(500);
    Serial.begin(115200);
    Serial.println("\nSunstone - BRC Wayfinder");

    // Disable power to the Feather ESP32-S2 TFT's onboard Neopixel LED
    pinMode(NEOPIXEL_POWER, OUTPUT);
    digitalWrite(NEOPIXEL_POWER, LOW);

    // Initialize TFT Display and Battery Monitor
    display.setup();

    // Initialize GPS
    gps.setup();

    // Initialize LoRa
    lora.setup();

    // Initialize GPS display refresh timer
    gpsDisplayUpdateTimer.start();

    // Initialize LoRa ping timer
    loraFindFriendTimer.start();

    Serial.println(F("Setup Complete!"));
    delay(200);
}

void loop() {
    gps.loop();
    display.loop();
    lora.loop();

    if (display.isOn()) {
      // Update displayed GPS location
      if(gpsDisplayUpdateTimer.hasPassed(displayUpdateGPSInterval, true)) {
        display.drawFix(gps.hasFix());
        display.drawTime(gps.time());
        display.drawLocation(gps.lat(), gps.lon());
      }
    }

    // Request the location of the first friend in our list
    if (loraFindFriendTimer.hasPassed(loraFindFriendInterval, true)) {
      lora.requestLocationFromDevice(friendAddresses[0]);
    }
}