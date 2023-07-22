#include <WiFi.h>
#include <WebServer.h>
//#include <SPIFFS.h> // SPIFFS = Serial Peripheral Interface Flash File System

//------------------------------- Ultrasonics -----------------------
#define TRIGGER_PIN 16 // TO DO
#define ECHO_PIN 5 // TO DO
//#define LED_PIN 5

// Datasheet Constants
#define MAX_DISTANCE 4.5 // m
#define SPEED_OF_SOUND 343 // m/s
#define TIME_OUT ((MAX_DISTANCE*2)/SPEED_OF_SOUND)/1000000 // us
#define MIN_TRIG_DURATION 10 // us
#define MIN_CYCLE_PERIOD 50 // ms

// Customisable Contants
#define WARNING_DISTANCE 0.1 // m

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
int enable1Pin = 32; 
int motor1Pin1 = 33; 
int motor1Pin2 = 25; 

// Motor B
int motor2Pin1 = 26; 
int motor2Pin2 = 27; 
int enable2Pin = 14; 

// Setting PWM properties
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 200;

bool LED1status;
bool LED2status;
typedef enum MotorControl { STOP, FWD, BACK, LEFT, RIGHT};
MotorControl motorControl = STOP;

//------------------------------- Setup ----------------------------
void setup() {
  // Configure Pins
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  //pinMode(LED_PIN, OUTPUT);
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

  /*
  #if 0
  // Move DC motor forward with increasing speed
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);
  while (dutyCycle <= 255){
    ledcWrite(pwmChannel, dutyCycle);   
    Serial.print("Forward with duty cycle: ");
    Serial.println(dutyCycle);
    dutyCycle = dutyCycle + 5;
    delay(500);
  }
  dutyCycle = 200;
  #endif
  */
}

//------------------------------- WiFI Functions ----------------------------
void handle_OnConnect() {

  Serial.println("onConnect");
  server.send(200, "text/html", SendHTML()); 
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
  server.send(200, "text/html", SendHTML()); 
}

void handle_fwd() {
  motorControl = FWD;
  Serial.println("FWD");
  server.send(200, "text/html", SendHTML()); 
}

void handle_back() {
  motorControl = BACK;
  Serial.println("BACK");
  server.send(200, "text/html", SendHTML()); 
}

void handle_left() {
  motorControl = LEFT;
  Serial.println("LEFT");
  server.send(200, "text/html", SendHTML()); 
}

void handle_right() {
  motorControl = RIGHT;
  Serial.println("RIGHT");
  server.send(200, "text/html", SendHTML()); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>ROVER Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
//  ptr +=".button-fwd {background-color: #3498db;}\n";
//  ptr +=".button-fwq:active {background-color: #2980b9;}\n";
//  ptr +=".button-stop {background-color: #34495e;}\n";
//  ptr +=".button-stop:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  
  ptr +="<body>\n";
  ptr +="<h1>ROVER Web Server</h1>\n";
  ptr +="<h3>Using Access Point(AP) Mode</h3>\n";
  
  if(motorControl == FWD)
  {
    ptr += "<p>Status: FORWARD</p>";
  }
  else if (motorControl == BACK)
  {
    ptr += "<p>Status: BACK</p>";
  }
  else if (motorControl == LEFT)
  {
    ptr += "<p>Status: LEFT</p>";
  }
    else if (motorControl == RIGHT)
  {
    ptr += "<p>Status: RIGHT</p>";
  }
  else
  {
    ptr += "<p>Status: STOPPED</p>";
  }

  ptr +="<a class=\"button button-fwd\" href=\"/fwd\">FWD</a>\n";
  ptr +="<a class=\"button button-stop\" href=\"/stop\">STOP</a>\n";
  ptr +="<a class=\"button button-back\" href=\"/back\">BACK</a>\n";
  ptr +="<a class=\"button button-left\" href=\"/left\">LEFT</a>\n";
  ptr +="<a class=\"button button-right\" href=\"/right\">RIGHT</a>\n";


  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
/*
void setupRoverInterface(){
  File file = SPIFFS.open(/"interface.html",r);
  if(!file){
    server.send(404, "File Not Found");
    return;
  }
  else{
    // Read the HTML file content into a String
    String htmlContent = "";
    while(file.available()){
      htmlContent += char(file.read()); // concatenate the contents of the HTML file to the variable
    }
    file.close();


  }
}*/


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
  digitalWrite(TRIGGER_PIN, LOW); // Ensure Trigger Pin initially LOW
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

void distanceToSerialMonitor(float distance){
  Serial.print("Object Detected at: ");
  Serial.print(distance);
  Serial.println(" m");
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
  delay()
  digitalWrite(pinNumber, LOW);
}