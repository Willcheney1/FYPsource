#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

/* Assign unique IDs to each sensor */
Adafruit_ADXL345_Unified accel1 = Adafruit_ADXL345_Unified(0x53); // Sensor with address 0x53
Adafruit_ADXL345_Unified accel2 = Adafruit_ADXL345_Unified(0x1D); // Sensor with address 0x1D

sensors_event_t prevEvent1, prevEvent2; // Store previous readings

float threshold1 = 10.0; // Set your threshold for sensor 1
float threshold2 = 1.0; // Set your threshold for sensor 2

void setup() {
  Serial.begin(9600);
  
  /* Initialize the sensors */
  if (!accel1.begin() || !accel2.begin()) {
    Serial.println("Ooops, no ADXL345 detected!");
    while (1);
  }
  
  /* Set the range and other settings if needed */
  accel1.setRange(ADXL345_RANGE_16_G);
  accel2.setRange(ADXL345_RANGE_4_G);
}

void loop() {
  /* Read data from the first sensor */
  sensors_event_t event1;
  accel1.getEvent(&event1);
  
  /* Read data from the second sensor */
  sensors_event_t event2;
  accel2.getEvent(&event2);

  /* Calculate the difference in acceleration values */
  float deltaX1 = event1.acceleration.x - prevEvent1.acceleration.x;
  float deltaY1 = event1.acceleration.y - prevEvent1.acceleration.y;
  float deltaZ1 = event1.acceleration.z - prevEvent1.acceleration.z;

  float deltaX2 = event2.acceleration.x - prevEvent2.acceleration.x;
  float deltaY2 = event2.acceleration.y - prevEvent2.acceleration.y;
  float deltaZ2 = event2.acceleration.z - prevEvent2.acceleration.z;

  /* Store the current readings as previous for the next iteration */
  prevEvent1 = event1;
  prevEvent2 = event2;

  /* Check if the deltas exceed the thresholds */
  if (abs(deltaX1) > threshold1 || abs(deltaY1) > threshold1 || abs(deltaZ1) > threshold1) 
  {
    Serial.println("Sensor 1 - Activity detected!");
  }
  if (abs(deltaX2) > threshold2 || abs(deltaY2) > threshold2 || abs(deltaZ2) > threshold2) 
  {
    Serial.println("Sensor 2 - Activity detected!");
  }

  delay(1000); // Adjust the delay as needed
}
