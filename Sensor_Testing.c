#include <NewPing.h>

#define Sensor1  A0  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define Sensor2  A1 // Arduino pin tied to echo pin on the ultrasonic sensor.
#define Sensor3  A2 
#define Sensor4  A3 
#define Sensor5  A4 
#define Sensor6  A5 
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

const int SONAR_NUM = 6;

NewPing sonar[SONAR_NUM] = {   // Sensor object array.
  NewPing(A0, A0, MAX_DISTANCE), // Each sensor's trigger pin, echo pin, and max distance to ping. 
  NewPing(A1, A1, MAX_DISTANCE), 
  NewPing(A2, A2, MAX_DISTANCE),
  NewPing(A3, A3, MAX_DISTANCE),
  NewPing(A4, A4, MAX_DISTANCE)
  NewPing(A5, A5, MAX_DISTANCE)
};

unsigned int cm[SONAR_NUM];



void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
}

void loop() {
  for (uint8_t i = 0; i < SONAR_NUM; i++) { // Loop through each sensor and display results.
    delay(50); // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
    Serial.print(i);
    Serial.print("=");
    cm[i]=sonar[i].ping_cm();
    Serial.print(cm[i]);
    Serial.print("cm ");
  }
  Serial.println();
}
