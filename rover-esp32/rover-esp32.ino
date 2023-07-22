#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h> // SPIFFS = Serial Peripheral Interface Flash File System

//------------------------------- Ultrasonics -----------------------
const int triggerPin = 16;
const int echoPin = 5;

// Datasheet Constants
const int maxDistance = 4.5; // m
const int speedOfSound = 343; // m/s
const int timeOut = ((maxDistance*2)/speedOfSound)/1000000; // us
const int minTrigDuration = 10; // us
const int minCyclePeriod = 50; // ms

// Customisable Contants
const int warningDistance = 0.1; // m

//------------------------------- WiFi ----------------------------

/* Put your SSID & Password */
const char* ssid = "ROVER-ESP32";  // Enter SSID here
const char* password = NULL; //"12345678";  //Enter Password here

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);

//------------------------------- Motors --------------------------
// Motor A
const int enable1Pin = 32; 
const int motor1Pin1 = 33; 
const int motor1Pin2 = 25; 

// Motor B
const int motor2Pin1 = 26; 
const int motor2Pin2 = 27; 
const int enable2Pin = 14; 

// Setting PWM properties
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
const int dutyCycle = 200;

bool LED1status;
bool LED2status;
typedef enum MotorControl { STOP, FWD, BACK, LEFT, RIGHT};
MotorControl motorControl = STOP;

//------------------------------- Setup ----------------------------
void setup() {
  // Configure Pins
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  pinMode(enable2Pin, OUTPUT);
  
  // configure LED PWM functionalitites
  // ledcSetup(pwmChannel, freq, resolution);
  // attach the channel to the GPIO to be controlled
  // ledcAttachPin(enable1Pin, pwmChannel);
  // ledcAttachPin(enable2Pin, pwmChannel);
  digitalWrite(enable1Pin, HIGH); // full speed
  digitalWrite(enable2Pin, HIGH);

  Serial.begin(115200);

  // testing
  Serial.print("Testing DC Motor...");
  
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  server.on("/", handle_OnConnect);
  server.on("/stop", handle_stop);
  server.on("/fwd", handle_fwd);
  server.on("/back", handle_back);
  server.on("/left", handle_left);
  server.on("/right", handle_right);
  server.onNotFound(handle_NotFound);

  server.begin();
}

//------------------------------- Loop ----------------------------
void loop() {
  server.handleClient();

  float distance = 0.0;
  distance = readUltrasonicSensor();
  distanceToSerialMonitor(distance);
  //ledIndication(distance);
  controlMotors(distance);
}

//------------------------------- WiFI Functions ----------------------------
void handle_OnConnect() {
  String htmlContent = "";
  File file = SPIFFS.open(/"Interface.html",r);

  Serial.println("onConnect");
  
  if(!file){
    Serial.println("Failed to find Interface.html");
  }
  else{
    Serial.println("Successfully found Interface.html");
    while(file.available()){
      Serial.println("Reading Interface.html");
      htmlContent += char(file.read()); // concatenate the contents of the HTML file to the variable
    }
    file.close();
  }
  
  server.send(200, "text/html", htmlContent);
}

//------------------------------- Motor Functions ----------------------------
void controlMotors(float distance){
 // Move the DC motor forward at maximum speed
  if(distance < WARNING_DISTANCE){
    if (motorControl == FWD)
    {
      Serial.println("Moving Forward");
      digitalWrite(motor1Pin1, HIGH);
      digitalWrite(motor1Pin2, LOW); 
      digitalWrite(motor2Pin1, HIGH);
      digitalWrite(motor2Pin2, LOW); 
    }
    else if (motorControl == BACK)
    {
      Serial.println("Moving Backwards");
      digitalWrite(motor1Pin1, LOW);
      digitalWrite(motor1Pin2, HIGH); 
      digitalWrite(motor2Pin1, LOW);
      digitalWrite(motor2Pin2, HIGH); 
    }
    else if (motorControl == LEFT)
    {
      Serial.println("Moving Left");
      digitalWrite(motor1Pin1, LOW);
      digitalWrite(motor1Pin2, HIGH); 
      digitalWrite(motor2Pin1, HIGH);
      digitalWrite(motor2Pin2, LOW); 
    }
    else if (motorControl == RIGHT)
    {
      Serial.println("Moving Right");
      digitalWrite(motor1Pin1, HIGH);
      digitalWrite(motor1Pin2, LOW); 
      digitalWrite(motor2Pin1, LOW);
      digitalWrite(motor2Pin2, HIGH); 
    }
    else
    {
      Serial.println("Motor stopped");
      digitalWrite(motor1Pin1, LOW);
      digitalWrite(motor1Pin2, LOW);
      digitalWrite(motor2Pin1, LOW);
      digitalWrite(motor2Pin2, LOW);    
    }

    delay(200);
  }
}

void handle_stop() {
  motorControl = STOP;
  Serial.println("STOP");
}

void handle_fwd() {
  motorControl = FWD;
  Serial.println("FWD");
}

void handle_back() {
  motorControl = BACK;
  Serial.println("BACK");
}

void handle_left() {
  motorControl = LEFT;
  Serial.println("LEFT");
}

void handle_right() {
  motorControl = RIGHT;
  Serial.println("RIGHT");
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

//------------------------------- Ultrasonic Functions ----------------------------
float readUltrasonicSensor() {
  unsigned long pulseDuration = 0.0;
  
  Serial.println("Scanning Environment...");
  sendTrigger();
  pulseDuration = receiveEcho();
  Serial.print("Pulse Duration: ");
  Serial.println(pulseDuration);
  return calculateDistance(pulseDuration);
}

void sendTrigger(){
  digitalWrite(triggerPin, LOW); // Ensure Trigger Pin initially LOW
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(minTrigDuration);
  digitalWrite(triggerPin, LOW);
}

unsigned long receiveEcho(){
  return pulseIn(echoPin, HIGH, timeOut);  // Measures pulse  
}

float calculateDistance(float pulseDuration){
  return (speedOfSound*(pulseDuration/1000000))/2;
}

void distanceToSerialMonitor(float distance){
  if(distance == 0.0){
    Serial.println("No Object Detected");
  }
  else{
    Serial.print("Object Detected at: ");
    Serial.print(distance);
    Serial.println(" m");
  }
}
/*
void ledIndication(float distance){
  if(distance < WARNING_DISTANCE){ // If object is less than 0.5m away, turn on LED   
    digitalWrite(LED_PIN, HIGH);
  }
  else{
    digitalWrite(LED_PIN, LOW);
  }
}*/

void testOutputPin(int pinNumber){
  pinMode(pinNumber, OUTPUT);
  digitalWrite(pinNumber, HIGH);
  delay(500);
  digitalWrite(pinNumber, LOW);
}