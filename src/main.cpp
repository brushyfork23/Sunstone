#include "Arduino.h"

// TFT Display
#include <TFT_eSPI.h>
#define PIN_POWER_ON 46
TFT_eSPI tft;

#include "resource/tautology_sign.h"

void setup()
{
    Serial.begin(115200);
    delay(100);

    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);

    tft.begin();
    tft.setRotation(3);

    tft.setSwapBytes(true);

    tft.fillScreen(TFT_BLACK);

    tft.pushImage(76, 0, 170, 167, tautology_sign);
}

void loop()
{
    Serial.println("Sunstone, ready to navigate!");
    delay(1000);
}