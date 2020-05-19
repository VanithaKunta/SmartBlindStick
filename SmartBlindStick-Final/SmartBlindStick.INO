#include <SoftwareSerial.h>
#include <VirtualWire.h>
#include <TinyGPS.h>

// PIN MAP
#define WATER 5
#define IR 4
#define US_1_ECHO 2
#define US_1_TRIGGER 3
#define RF 19
#define BUZZER 13
#define US_2_ECHO 6
#define US_2_TRIGGER 7
#define GPS_RX 16 // A2
#define GPS_TX 17 // A3
#define OBSTACLE_AUDIO 8
#define WATER_AUDIO 9
#define STAIRCASE_AUDIO 10
#define OBJECT_AUDIO 11
#define VIBRATOR 18
#define GSM_RX 14 // A0
#define GSM_TX 15 // A1
#define BUTTON 12
#define DEFAULT_MESSAGE "I'm in danger please find me here http://maps.google.com/maps?q=loc:"

// Obstacle type

#define STAIRCASE 111
#define SMALL_OBSTACLE 222
#define LARGE_OBSTACLE 333

SoftwareSerial GPSModule(GPS_RX, GPS_TX);
TinyGPS gpsObject;
SoftwareSerial GSMModule(GSM_RX, GSM_TX);
long lat, lon;
float flat, flon;
unsigned long age;

byte RFSignal[VW_MAX_MESSAGE_LEN]; // a buffer to store the incoming messages
byte RFSignalMaxLength = VW_MAX_MESSAGE_LEN; // the size of the message

/** SETUP **/
void setupDebugger()
{
  Serial.begin(9600);
}

void setupIRSensor(){
  pinMode(IR,INPUT);
}

void setupWaterSensor(){
  pinMode(WATER,INPUT);
}

void setupNearUltrasonicSensor(){
  Serial.println("Initializing UltrasonicSensor");
  pinMode(US_1_TRIGGER, OUTPUT);
  pinMode(US_1_ECHO, INPUT); 
}

void setupRFModule(){
  Serial.println("Initializing RFModule");
  vw_set_rx_pin(RF);
  vw_setup(2000); // Bits per sec
  vw_rx_start(); // Start the receiver
}

void setupBuzzer(){
  Serial.println("Initializing BUZZER");
  pinMode(BUZZER,OUTPUT);
}

void setupFarUltrasonicSensor(){
  Serial.println("Initializing Far Ultrasonic Sensor");
  pinMode(US_2_ECHO,INPUT);
}

void setupGPSModule(){
  Serial.println("Initializing GPSModule");
  GPSModule.begin(9600);
  delay(1000);
}

void setupSpeaker(){
  Serial.println("Initializing Speaker");
  pinMode(OBSTACLE_AUDIO,OUTPUT);
  pinMode(WATER_AUDIO,OUTPUT);
  pinMode(STAIRCASE_AUDIO,OUTPUT);
  pinMode(OBJECT_AUDIO,OUTPUT);
}

void setupVibrationMotor(){
  Serial.println("Initializing BUTTON");
  pinMode(VIBRATOR,OUTPUT);
}

void setupGSMModule(){
  Serial.println("Initializing GSMModule");
  GSMModule.begin(9600); 
  delay(1000);
}

void setupBUTTON(){
  Serial.println("Initializing BUTTON");
  pinMode(BUTTON, INPUT_PULLUP);
}


int getNearSensorDistance(){
  digitalWrite(US_1_TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(US_1_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(US_1_TRIGGER, LOW);
  long duration = pulseIn(US_1_ECHO, HIGH);
  int distance = duration * 0.034/2;
  return distance;
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}

int getFarSensorDistance(){
  pinMode(US_2_ECHO, OUTPUT);
  digitalWrite(US_2_ECHO, LOW);
  delayMicroseconds(2);
  digitalWrite(US_2_ECHO, HIGH);
  delayMicroseconds(5);
  digitalWrite(US_2_ECHO, LOW);
  pinMode(US_2_ECHO, INPUT);
  long duration = pulseIn(US_2_ECHO, HIGH);
  int cm = microsecondsToCentimeters(duration);
  return cm;
}

void checkConnectivity()
{
  debugGSMModule();
  GSMModule.println("AT");
  debugGSMModule();
  delay(1000);
  debugGSMModule();
}

void sendMessage(String message)
{
  Serial.println("sending message");
  GSMModule.listen();
  GSMModule.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  debugGSMModule();
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  debugGSMModule();
  GSMModule.println("AT+CMGS=\"+918121066735\"\r"); // Replace x with mobile number
  debugGSMModule();
  delay(1000);
  debugGSMModule();
  GSMModule.println(message);// The SMS text you want to send
  delay(100);
  debugGSMModule();
  GSMModule.println((char)26);// ASCII code of CTRL+Z
  debugGSMModule();
  delay(1000);
  debugGSMModule();
  Serial.println("done sending message");
}

void updateGPSLocation()
{
  GPSModule.listen();
  unsigned long startTime = millis();
  while (millis() - startTime < 5000) 
  {
    if (GPSModule.available()) 
    {
        char c = GPSModule.read();
        //Serial.print(c);  // uncomment to see raw GPS data
        if (gpsObject.encode(c)) 
        {
            break;  // uncomment to print new data immediately!
        }
    }
    else
    {
      Serial.println("Getting GPS Coordinates");
    }
  }
  gpsObject.get_position(&lat, &lon, &age);
  gpsObject.f_get_position(&flat, &flon, &age);
  return;
}

void debugGSMModule(){
   GSMModule.listen();
   if (GSMModule.available()){
    Serial.write(GSMModule.read());
  }
}

bool checkIRSensor(){
   if (digitalRead(IR)== LOW){
      return true;
   } 
   return false;
}

bool checkWater(){
   if (digitalRead(WATER)== LOW){
      return true;
   } 
   return false;
}

bool checkRFMessage(){
  //Serial.println("Checking for RF message");
  if (vw_get_message(RFSignal, &RFSignalMaxLength)) 
  {
     Serial.println(RFSignal[0]);
     return true;
  }
  return false;
}

bool checkButtonPress(){
   int buttonValue = digitalRead(BUTTON);
   if (buttonValue == LOW){
      Serial.println("Button pressed");
      return true;
   } 
   return false;
}

void raiseAlert(int SIGNAL, int duration){
  long startTime = millis();
  while(millis() - startTime < duration)
  {
    digitalWrite(SIGNAL, HIGH);
    //digitalWrite(VIBRATOR, HIGH);
  }
  digitalWrite(SIGNAL, LOW);
  return;
  //digitalWrite(VIBRATOR, LOW);
}

int checkObstacle(){
  int d1 = getNearSensorDistance();
  int d2 = getFarSensorDistance();
  bool ir = checkIRSensor();
  Serial.print("Distance of Sensors: ");
  Serial.print(d1);
  Serial.print(" ");
  Serial.println(d2);
  if( d1  <= 50 && d2 <= 75){
      return LARGE_OBSTACLE; 
  }
  if( d1 <= 50){
      return SMALL_OBSTACLE;
  }
  Serial.println("No obstacle found");
  return -1;

}

void setup() {
  setupDebugger();
  setupWaterSensor();
  setupIRSensor();
  setupNearUltrasonicSensor(); 
  setupRFModule();
  setupBuzzer();
  setupFarUltrasonicSensor();
  setupGPSModule();
  setupSpeaker();
  setupVibrationMotor();
  setupGSMModule();
  setupBUTTON();
}

void loop() {
  if(checkRFMessage()){
    raiseAlert(BUZZER,2000);
    Serial.println("received");
  }  
  if(checkButtonPress()){
    updateGPSLocation();
    String message = DEFAULT_MESSAGE + String(flat,6) + String(",") + String(flon,6);
    Serial.println(message);
    sendMessage(message);
  }
  
//  if(checkWater()){
//    raiseAlert(VIBRATOR,2000);
//    Serial.println("water");
//  }
  switch(checkObstacle()){
    case STAIRCASE:
      //raiseAlert(STAIRCASE_AUDIO,100);
      Serial.println("STAIRCASE");
      break;
    case SMALL_OBSTACLE:
      raiseAlert(BUZZER,100);
      raiseAlert(BUZZER,100);
      Serial.println("SMALL OBSTACLE");
      break;
    case LARGE_OBSTACLE:
      raiseAlert(BUZZER,500);
      Serial.println("LARGE OBSTACLE");
      break;
    case DEFAULT:
      break;
  }
  delay(100);
}
