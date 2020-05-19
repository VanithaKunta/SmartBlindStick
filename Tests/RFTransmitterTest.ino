#include <VirtualWire.h>

int button[4] = {4,5,6,7};
char* arr[4] = {"1","2","3","4"};

void setup()
{
  // Initialize the IO and ISR
  for(int i = 0; i < 4; i++){
    pinMode(button[i], INPUT_PULLUP);
  }
  pinMode(13, OUTPUT);
  vw_setup(2000); // Bits per sec
  vw_set_tx_pin(12);
  Serial.begin(9600);
}

void loop()
{
  for(int i = 0; i < 4; i++){
    if(digitalRead(button[i]) == LOW){
        digitalWrite(13,HIGH);
        Serial.print("sending value: ");
        send("hello");
        delay(100);
    }
    digitalWrite(13,LOW);
  }
}

void send (char *message)
{
  vw_send((uint8_t *)message, strlen(message));
  vw_wait_tx(); // Wait until the whole message is gone
  Serial.println("hello");
}
