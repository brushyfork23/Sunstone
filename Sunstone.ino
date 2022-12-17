// Sunstone
// Handheld Black Rock City Wayfinder

// Upload this file to the Adafruit ESP32-S2 TFT Feather - http://www.adafruit.com/products/5300
// Connect to the Adafruit GPS FeatherWing - https://www.adafruit.com/products/3133

// Use Arduino Board Manager to install the required libraries (and all dependencies):
// Adafruit LC709203F - Battery Monitor
// Adafruit ST7735 and ST7789 Library - TFT Display
// Adafruit GPS Library - GPS

// Manually install this library:
// http://www.airspayce.com/mikem/arduino/RadioHead/ - LoRa

// Default list of friends' device addresses
uint8_t friendAddresses[] = {2};

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
// How often the GPS coordinates and clock are updated,
// _not_ how often they are fetched and parsed.
#define GPS_TIMER_MICROS 1000000 // 1 second
hw_timer_t* gpsTimer = NULL;
volatile SemaphoreHandle_t gpsTimerSemaphore;
void ARDUINO_ISR_ATTR onGpsTimer(){
  xSemaphoreGiveFromISR(gpsTimerSemaphore, NULL);
}

// LoRa friend ping timer
// How often we query our first friend for their location.
#define LORA_TIMER_MICROS 5000000 // 50 seconds
hw_timer_t* loraTimer = NULL;
volatile SemaphoreHandle_t loraTimerSemaphore;
void ARDUINO_ISR_ATTR onLoraTimer(){
  xSemaphoreGiveFromISR(loraTimerSemaphore, NULL);
}

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
    gpsTimerSemaphore = xSemaphoreCreateBinary();
    gpsTimer = timerBegin(1, 80, true); // prescaler of 80 configures alarm time to microseconds
    timerAttachInterrupt(gpsTimer, &onGpsTimer, true);
    timerAlarmWrite(gpsTimer, GPS_TIMER_MICROS, true);
    timerAlarmEnable(gpsTimer);

    // Initialize LoRa ping timer
    loraTimerSemaphore = xSemaphoreCreateBinary();
    loraTimer = timerBegin(2, 80, true); // prescaler of 80 configures alarm time to microseconds
    timerAttachInterrupt(loraTimer, &onLoraTimer, true);
    timerAlarmWrite(loraTimer, LORA_TIMER_MICROS, true);
    timerAlarmEnable(loraTimer);

    Serial.println(F("Setup Complete!"));
    delay(200);
}

void loop() {
    gps.loop();
    display.loop();
    lora.loop();

    if (display.isOn()) {
      // Update displayed GPS location
      if (!timerStarted(gpsTimer)) {
        timerStart(gpsTimer);
      }
      if (xSemaphoreTake(gpsTimerSemaphore, 0) == pdTRUE) {
        display.drawFix(gps.hasFix());
        display.drawTime(gps.time());
        display.drawLocation(gps.lat(), gps.lon());
      }
    }

    // Request the location of the first friend in our list
    if (!timerStarted(loraTimer)) {
      timerStart(loraTimer);
    }
    if (xSemaphoreTake(loraTimerSemaphore, 0) == pdTRUE) {
      lora.requestLocationFromDevice(friendAddresses[0]);
    }
}