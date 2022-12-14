#ifndef GPS_h
#define GPS_h

#include <Adafruit_GPS.h>
#define GPSSerial Serial1 // RX: A0, TX: A1

class GPS {

private:
    Adafruit_GPS gps = Adafruit_GPS(&GPSSerial);

public:
    void setup() {
        Serial.println(F("Initializing GPS"));
        gps.begin(9600);
        // request RMC (recommended minimum) and GGA (fix data) including altitude.
        // Can we get away with just PMTK_SET_NMEA_OUTPUT_RMCONLY (minimum recommended only)?
        gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
        gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
        Serial.println(F("GPS Initialized!"));
    }

    void loop() {
        // Read new character from GPS module
        gps.read();

        // Parse new data
        if(gps.newNMEAreceived()) {
          gps.parse(gps.lastNMEA());
        }
    }

    bool hasFix() {
        return gps.fix;
    }

    String lat() {
        return String(
            String(gps.latitude, 4) +
            String(gps.lat)
        );
    }

    String lon() {
        return String(
            String(gps.longitude, 4) +
            String(gps.lon)
        );
    }

    String time() {
        return String(
            String((gps.hour < 10 ? "0" : "")) +
            gps.hour +
            String(":") +
            String((gps.minute < 10 ? "0" : "")) +
            gps.minute +
            String(":") +
            String((gps.seconds < 10 ? "0" : "")) +
            gps.seconds
        );
    }
};

#endif