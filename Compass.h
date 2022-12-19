#ifndef Compass_h
#define Compass_h

// Compass Sensor Read Timer
#define COMPASS_TIMER_ID 1 // Must be unique amongst all hardware timers
#define COMPASS_TIMER_MICROS 10000 // read magnetometer 10 miliseconds 
#define SENSOR_ID 12345

#include <Adafruit_LIS2MDL.h>

// Compass Sensor Read Timer
hw_timer_t* compassTimer = NULL;
volatile SemaphoreHandle_t compassTimerSemaphore;
void ARDUINO_ISR_ATTR onCompassTimer(){
    xSemaphoreGiveFromISR(compassTimerSemaphore, NULL);
}

class Compass {

private:
    Adafruit_LIS2MDL mag = Adafruit_LIS2MDL(SENSOR_ID);
    const float Pi = 3.14159;

public:
    bool enabled = true;
    float heading = 0;

    void setup() {
        Serial.println(F("Initializing Compass"));
        // Initialise the sensor
        if(!mag.begin()) {
          Serial.println(F("WARNING: Compass begin Failed!"));
          return;
        }
        // Set up recurring query of magnetometer
        compassTimerSemaphore = xSemaphoreCreateBinary();
        compassTimer = timerBegin(COMPASS_TIMER_ID, 80, true); // prescaler of 80 configures alarm time to microseconds
        timerAttachInterrupt(compassTimer, &onCompassTimer, true);
        timerAlarmWrite(compassTimer, COMPASS_TIMER_MICROS, true);
        timerAlarmEnable(compassTimer);
        Serial.println(F("LC709203F Battery Monitor Initialized!"));
        Serial.println(F("Compass Initialized!"));
    }

    void loop() {
      // Update heading
      if (enabled) {
        if(!timerStarted(compassTimer)) {
          timerStart(compassTimer);
        }
        if (xSemaphoreTake(compassTimerSemaphore, 0) == pdTRUE){
            // Read sensor
            sensors_event_t event;
            mag.getEvent(&event);

            // Calculate the angle of the vector y,x
            heading = (atan2(event.magnetic.y,event.magnetic.x) * 180) / Pi;

            // Normalize to 0-360
            if (heading < 0) {
              heading = 360 + heading;
            }
        }
      }
    }
};

#endif