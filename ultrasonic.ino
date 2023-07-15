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
#define TRIGGER_PIN 4 // TO DO
#define ECHO_PIN 2 // TO DO

// Datasheet Constants
#define MAX_DISTANCE 4.5 // m
#define SPEED_OF_SOUND 343 // m/s
#define TIME_OUT (MAX_DISTANCE*2)/SPEED_OF_SOUND // s
#define MIN_TRIG_DURATION 10 // us
#define MIN_CYCLE_PERIOD 50 // ms

void setup() {
  
  // Pin configuration:
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.begin(115200);

}

void loop() {
  readUltrasonicSensor();
  delay(MIN_CYCLE_PERIOD); // ms
}

void readUltrasonicSensor() {
  float timeSent = 0.0;
  float timeReceived = 0.0;
  float distance = 0.0;

  Serial.println("Scanning Environment...");
  timeSent = sendTrigger();
  timeReceived = receiveEcho(timeSent);
  distance = calculateDistance(timeSent, timeReceived);

  if(distance > 0.0){
    Serial.println("Object Detected at " + String(distance) + " m");
  }
  else{
    Serial.println("No Objects Detected...");
  }
  
}

float sendTrigger(){
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(MIN_TRIG_DURATION);
  digitalWrite(TRIGGER_PIN, LOW);
  return millis();
}

float receiveEcho(float timeSent){
  float timeElapsed = 0.0;
  float currentTime = 0.0;
  
  while(timeElapsed < TIME_OUT){ 
    
    if(!digitalRead(ECHO_PIN)){ // ECHO_PIN reads HIGH until it receives a reflected signal
      return millis();
    }
    currentTime = millis();
    timeElapsed = (currentTime - timeSent)*0.001; // s
  }
  return timeSent; // Set timeReceived to a default value if no reflected signal is received
}

float calculateDistance(float timeSent, float timeReceived){

  if(timeReceived > timeSent){
    return (SPEED_OF_SOUND*(timeReceived - timeSent))/2;
  }
  else{
    return 0;
  }
}
