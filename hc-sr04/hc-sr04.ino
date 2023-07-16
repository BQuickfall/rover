// Author: Ben Quickfall
// Date: 15th July 2023
// Description: This file provides the functionality to use the ultrasonic sensor (model: HC-SR04) to detect distance to objects
// NB. Arduino IDE was used and the Board selected was ESP32 Dev Module
//
// HC-SRO4 Data Sheet Specs:
// - Model: HC-SR04
// - Pins: Vcc, Trig, Echo, Grnd
// - Power Supply: 5V DC
// - Quiescent Current: <2mA 
// - Measuring (Effectual) Angle: +-<15°
// - Theoretical Measuring Distance: 2cm to 450cm
// - Practical Measuring Distance: 2cm to 80cm
// - Resolution/Accuracy: 3mm
// - Minimum Cycle Period: 50 ms
// Amazon Link: https://www.amazon.co.uk/ELEGOO-Ultrasonic-Raspberry-Datasheet-Available/dp/B01M0QL1F1/ref=sr_1_2_sspa?keywords=HC-SR04&qid=1689429181&sr=8-2-spons&sp_csd=d2lkZ2V0TmFtZT1zcF9hdGY&psc=1

// Pin Assignments
#define TRIGGER_PIN 3 // TO DO
#define ECHO_PIN 2 // TO DO
#define LED_PIN 5

// Datasheet Constants
#define MAX_DISTANCE 4.5 // m
#define SPEED_OF_SOUND 343 // m/s
#define TIME_OUT ((MAX_DISTANCE*2)/SPEED_OF_SOUND)/1000000 // us
#define MIN_TRIG_DURATION 10 // us
#define MIN_CYCLE_PERIOD 50 // ms

// Customisable Contants
#define WARNING_DISTANCE 0.1 // m

void setup() {
  
  // Pin configuration:
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);

}

void loop() {
  float distance = 0.0;
  distance = readUltrasonicSensor();
  distanceToSerialMonitor(distance);
  ledIndication(distance);
  delay(MIN_CYCLE_PERIOD); 
}

float readUltrasonicSensor() {
  unsigned long pulseDuration = 0.0;
  
  Serial.println("Scanning Environment...");
  sendTrigger();
  pulseDuration = receiveEcho();
  return calculateDistance(pulseDuration);
}

void distanceToSerialMonitor(float distance){
  Serial.print("Object Detected at: ");
  Serial.print(distance);
  Serial.println(" m");
}

void ledIndication(float distance){
  if(distance < WARNING_DISTANCE){ // If object is less than 0.5m away, turn on LED   
    digitalWrite(LED_PIN, HIGH);
  }
  else{
    digitalWrite(LED_PIN, LOW);
  }
}

void sendTrigger(){
  digitalWrite(TRIGGER_PIN, LOW); // Ensure Trigger Pin initially LOW
  delay(2); // 2ms delay
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(MIN_TRIG_DURATION);
  digitalWrite(TRIGGER_PIN, LOW);
}

unsigned long receiveEcho(){
  return pulseIn(ECHO_PIN, HIGH, TIME_OUT);  // Measures pulse  
}

float calculateDistance(float pulseDuration){
  return (SPEED_OF_SOUND*(pulseDuration/1000000))/2;
}
