#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LIS2MDL.h>
#include <Adafruit_LSM303_Accel.h>
#include "orientation.h"

// For the fabs function in fixedWidthPrint
#include <Math.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_LSM303_Accel_Unified accl = Adafruit_LSM303_Accel_Unified(54321);
Adafruit_LIS2MDL magn = Adafruit_LIS2MDL(12345);

void displaySensorDetails(void)
{
  sensor_t sensor;
  accl.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); 
  Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); 
  Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); 
  Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); 
  Serial.print(sensor.max_value); 
  Serial.println(" m/s^2");
  Serial.print  ("Min Value:    "); 
  Serial.print(sensor.min_value); 
  Serial.println(" m/s^2");
  Serial.print  ("Resolution:   "); 
  Serial.print(sensor.resolution); 
  Serial.println(" m/s^2");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void setup(void) 
{
  Serial.begin(115200);
  delay(500);
  Serial.println("Tilt Compensated Compass  Test"); 
  Serial.println("");

  /* Initialise the sensor */
  if(!accl.begin() || !magn.begin())  // also initalizes mag and accel sensors
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }

  /* Enable auto-gain */
  magn.enableAutoRange(true);
  /* Display some basic information on this sensor */
  displaySensorDetails();
  
  
  // Seattle, WA geomagnetic field
  // Declination +15
  float declination = 15;

  // from calibration using https://learn.adafruit.com/adafruit-sensorlab-magnetometer-calibration
  float hardiron_x = -2.84;
  float hardiron_y = -18.78;
  float hardiron_z = 0.91;
  
  Orientation::setParameters(&accl, &magn, declination, hardiron_x, hardiron_y, hardiron_z);
}

void fixedWidthPrint(float value){
  // Bug: one too many spaces when value === 10
  int characters = 0;
   
  // Count the number of digits before the decimal point
  for(int i = 10000; i > 0; i /= 10){
     if((int)fabs(value) >= i){
       characters ++;
     }
  } 
  
  if(characters == 0){
    characters = 1; // Minimum of 1 character if we print '0'
  }
  
  if(fabs(value) != value){ // Is it negative?
    characters++;
  }
  
  for(int i = 6; i > characters; i--){
    Serial.print(' ');
  }
  Serial.print(value, 2);
}

void loop(void) 
{ 
  float roll;
  float pitch;
  float yaw;
  float heading;

  Orientation::calculate(roll, pitch, yaw, heading);
 
  Serial.print(" | roll:"); fixedWidthPrint(roll);
  Serial.print(" | pitch:"); fixedWidthPrint(pitch);  
  Serial.print(" | yaw:"); fixedWidthPrint(yaw);
  Serial.print(" | heading:"); fixedWidthPrint(heading);
  
  Serial.print('\n');
  delay(50);
}