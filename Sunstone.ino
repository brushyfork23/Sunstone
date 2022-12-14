// Sunstone
// Handheld Black Rock City navigation system

// Upload this file to the Adafruit ESP32-S2 TFT Feather - http://www.adafruit.com/products/5300
// Connect to the Adafruit GPS FeatherWing - https://www.adafruit.com/products/3133

// Use Arduino Board Manager to install the required libraries (and all dependencies):
// Battery Monitor - Adafruit LC709203F
// TFT Display - Adafruit ST7735 and ST7789 Library
// GPS - Adafruit GPS Library

// Display
#include "Display.h"
Display display;

// GPS
#include "GPS.h"
GPS gps;

// GPS display refresh timer
#define GPS_TIMER_MICROS 1000000 // 1 second
hw_timer_t* gpsTimer = NULL;
volatile SemaphoreHandle_t gpsTimerSemaphore;
void ARDUINO_ISR_ATTR onGpsTimer(){
  xSemaphoreGiveFromISR(gpsTimerSemaphore, NULL);
}

void setup() {
    delay(500);
    Serial.begin(115200);
    Serial.println("\nSunstone Navigation System");

    // Disable power to the Feather ESP32-S2 TFT's onboard Neopixel LED
    pinMode(NEOPIXEL_POWER, OUTPUT);
    digitalWrite(NEOPIXEL_POWER, LOW);

    // Initialize TFT Display and Battery Monitor
    display.setup();

    // Initialize GPS
    gps.setup();

    // Initialize GPS display refresh timer
    gpsTimerSemaphore = xSemaphoreCreateBinary();
    gpsTimer = timerBegin(1, 80, true); // prescaler of 80 configures alarm time to microseconds
    timerAttachInterrupt(gpsTimer, &onGpsTimer, true);
    timerAlarmWrite(gpsTimer, GPS_TIMER_MICROS, true);
    timerAlarmEnable(gpsTimer);

    Serial.println(F("Setup Complete!"));
    delay(200);
}

void loop() {
    gps.loop();
    display.loop();

    // Update displayed GPS location
    if (display.isOn()) {
      if (!timerStarted(gpsTimer)) {
        timerStart(gpsTimer);
      }
      if (xSemaphoreTake(gpsTimerSemaphore, 0) == pdTRUE) {
        display.drawFix(gps.hasFix());
        display.drawTime(gps.time());
        display.drawLocation(gps.lat(), gps.lon());
      }
    }
}