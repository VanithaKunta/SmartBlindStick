#define OBSTACLE_AUDIO 8
#define WATER_AUDIO 9
#define STAIRCASE_AUDIO 10
#define OBJECT_AUDIO 11

void setupSpeaker(){
  Serial.println("Initializing Speaker");
  pinMode(OBSTACLE_AUDIO,OUTPUT);
  pinMode(WATER_AUDIO,OUTPUT);
  pinMode(STAIRCASE_AUDIO,OUTPUT);
  pinMode(OBJECT_AUDIO,OUTPUT);
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

void setup() {
   Serial.begin(9600);
   setupSpeaker();

}

void loop() {
//  raiseAlert(8,200);
//  raiseAlert(9,200);
//  raiseAlert(10,200);
//  raiseAlert(11,200);
  digitalWrite(8,HIGH);
  delay(1000);
  digitalWrite(8,LOW);
  digitalWrite(9,HIGH);
  delay(1000);
  digitalWrite(9,LOW);
  digitalWrite(10,HIGH);
  delay(1000);
  digitalWrite(10,LOW);
  digitalWrite(11,HIGH);
  delay(1000);
  digitalWrite(11,LOW);

}
