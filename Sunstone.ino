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
#include <Adafruit_GPS.h>
#define GPSSerial Serial1 // RX: A0, TX: A1
Adafruit_GPS GPS(&GPSSerial);
// GPS Display Update Timer
#define GPS_TIMER_MILLIS 500000 // 5 seconds
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
    display.print("Sunstone");

    // Initialize GPS
    Serial.println(F("Initializing GPS"));
    GPS.begin(9600);
    // request RMC (recommended minimum) and GGA (fix data) including altitude.
    // Can we get away with just PMTK_SET_NMEA_OUTPUT_RMCONLY (minimum recommended only)?
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
    // Initialize GPS Timer
    gpsTimerSemaphore = xSemaphoreCreateBinary();
    gpsTimer = timerBegin(0, 800, true); // prescaler of 800 configures alarm time to milliseconds
    timerAttachInterrupt(gpsTimer, &onGpsTimer, true);
    timerAlarmWrite(gpsTimer, GPS_TIMER_MILLIS, true);
    timerAlarmEnable(gpsTimer);
    Serial.println(F("GPS Initialized!"));

    Serial.println(F("Setup Complete!"));
    delay(200);
}

void loop() {
    // Read GPS
    GPS.read();

    display.loop();

    // Update displayed GPS location
    if (display.isOn()) {
      if (!timerStarted(gpsTimer)) {
        timerStart(gpsTimer);
      }
      if (xSemaphoreTake(gpsTimerSemaphore, 0) == pdTRUE) {
        // fetch the latest GPS data
        Serial.print(F("Time: "));
        if (GPS.hour < 10) { Serial.print('0'); }
        Serial.print(GPS.hour, DEC); Serial.print(':');
        if (GPS.minute < 10) { Serial.print('0'); }
        Serial.print(GPS.minute, DEC); Serial.print(':');
        if (GPS.seconds < 10) { Serial.print('0'); }
        Serial.print(GPS.seconds, DEC); Serial.print(F(" "));
        Serial.print("Date: ");
        Serial.print(GPS.day, DEC); Serial.print('/');
        Serial.print(GPS.month, DEC); Serial.print("/20");
        Serial.println(GPS.year, DEC);
        Serial.print("Fix: "); Serial.print((int)GPS.fix);
        Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
        if (GPS.fix) {
          Serial.print("Location: ");
          Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
          Serial.print(", ");
          Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
          Serial.print("Speed (knots): "); Serial.println(GPS.speed);
          Serial.print("Angle: "); Serial.println(GPS.angle);
          Serial.print("Altitude: "); Serial.println(GPS.altitude);
          Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
          Serial.print("Antenna status: "); Serial.println((int)GPS.antenna);
        }
        Serial.println();
      }
    }
}