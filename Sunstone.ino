// Sunstone
// Handheld Black Rock City navigation system

// Upload this file to the Adafruit ESP32-S2 TFT Feather - http://www.adafruit.com/products/5300

// Use Arduino Board Manager to install the required libraries (and all dependencies):
// Battery Monitor - Adafruit LC709203F
// TFT Display - Adafruit ST7735 and ST7789 Library

// Display
#include "Display.h"
Display display;

void setup() {
    delay(500);
    Serial.begin(115200);
    Serial.println("\nSunstone Navigation System");

    // Disable power to the Feather ESP32-S2 TFT's onboard Neopixel LED
    pinMode(NEOPIXEL_POWER, OUTPUT);
    digitalWrite(NEOPIXEL_POWER, LOW);

    // Initialize TFT Display and Battery Monitor
    display.setup();
    display.print("Sunstone");

    Serial.println(F("Setup Complete!"));
    delay(500);
}

void loop() {
  display.loop();
}