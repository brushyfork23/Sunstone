#ifndef Display_h
#define Display_h

// Battery Monitor Refresh Timer
#define BATTERY_TIMER_MICROS 5000000 // update battery display every 5 seconds

// Battery Monitor
#include "Adafruit_LC709203F.h"

// TFT Display
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

// Battery Monitor Refresh Timer
hw_timer_t* batteryTimer = NULL;
volatile SemaphoreHandle_t batteryTimerSemaphore;
void ARDUINO_ISR_ATTR onBatteryTimer(){
    xSemaphoreGiveFromISR(batteryTimerSemaphore, NULL);
}

class Display {

private:
    // Battery Monitor
    const lc709203_adjustment_t BATTERY_PACK_SIZE = LC709203F_APA_500MAH;
    Adafruit_LC709203F battery;
    float batteryVoltage = 0.0F;
    
    // TFT Display
    Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
    bool displayOn = true;

public:
    void setup() {
        // Enable I2C power for both the battery monitor and the TFT display
        pinMode(TFT_I2C_POWER, OUTPUT);
        digitalWrite(TFT_I2C_POWER, HIGH);
        delay(10);

        // Initialize Battery Monitor
        Serial.println(F("Initializing LC709203F Battery Monitor"));
        if (!battery.begin()) {
            Serial.println(F("WARNING: Failed to connect to Adafruit LC709203F over I2C.  No battery monitor available."));
        }
        Serial.print("Version: 0x"); Serial.println(battery.getICversion(), HEX);
        battery.setPackSize(BATTERY_PACK_SIZE);
        // Set up recurring query of battery voltage
        batteryTimerSemaphore = xSemaphoreCreateBinary();
        batteryTimer = timerBegin(0, 80, true); // prescaler of 80 configures alarm time to microseconds
        timerAttachInterrupt(batteryTimer, &onBatteryTimer, true);
        timerAlarmWrite(batteryTimer, BATTERY_TIMER_MICROS, true);
        timerAlarmEnable(batteryTimer);
        Serial.println(F("LC709203F Battery Monitor Initialized!"));

        // Initialize TFT Display
        Serial.println(F("Initializing TFT Display"));

        // turn on the backlite
        pinMode(TFT_BACKLITE, OUTPUT);
        digitalWrite(TFT_BACKLITE, HIGH);
        
        // configure screen size and rotation
        tft.init(135, 240);
        tft.setRotation(3);
        tft.fillScreen(ST77XX_BLACK);
        tft.setTextColor(ST77XX_WHITE);
        tft.setTextSize(1);
        tft.setTextWrap(true);
        tft.setCursor(0, 8);
        Serial.println(F("TFT Display Initialized!"));
    }

    void loop() {
        // Read Battery Monitor
        if (displayOn) {
            if(!timerStarted(batteryTimer)) {
                timerStart(batteryTimer);
            }
            if (xSemaphoreTake(batteryTimerSemaphore, 0) == pdTRUE){
                drawBatteryIndicator();
            }
        }
    }

    void drawBatteryIndicator() {
        // Serial.print("Batt_Voltage:");
        // Serial.print(battery.cellVoltage(), 3);
        // Serial.print("\t");
        // Serial.print("Batt_Percent:");
        // Serial.print(battery.cellPercent(), 1);
        // Serial.print("\t");
        // Serial.print("Batt_Temp:");
        // Serial.println(battery.getCellTemperature(), 1);

        float percent = battery.cellPercent();
        
        uint8_t digits = 0;
        if (percent < 9.5) {
            digits = 1;
        } else if (percent < 99.5) {
            digits = 2;
        } else {
            digits = 3;
        }

        uint16_t color;
        if (percent < 10) {
            color = ST77XX_RED;
        } else if (percent < 33) {
            color = ST77XX_YELLOW;
        } else if (percent < 90) {
            color = ST77XX_WHITE;
        } else {
            color = ST77XX_GREEN;
        }

        uint8_t fillWidth = map(percent, 0, 100, 0, 21);

        // Write battery percent
        if (digits < 3) {
            tft.fillRect(166,0,(3-digits)*12,12,ST77XX_BLACK);
        }
        tft.setCursor(214 - ((digits+1) * 12), 0);
        tft.setTextColor(color, ST77XX_BLACK);
        tft.setTextSize(2);
        tft.print(percent, 0);
        tft.print("%");
        // Draw battery
        tft.drawRect(216,0,24,14,color);
        tft.fillRect(217,1,22,12,ST77XX_BLACK);
        tft.fillRect(217,1,fillWidth,12,color);
    }

    void print(const String &s) {
        tft.print(s);
    }
};

#endif