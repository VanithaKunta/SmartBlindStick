/*********************
 *10 to GPS Module TX*
 *09 to GPS Module RX*
 *********************/

#include <SoftwareSerial.h>
#include <TinyGPS.h>

SoftwareSerial GPSModule(16, 17);
TinyGPS gpsObject;
long lat, lon;
float flat, flon;
unsigned long age;

void gpsdump(TinyGPS &gps);
void printFloat(double f, int digits = 2);

void setup()  
{
  Serial.begin(9600);
  GPSModule.begin(9600);
  delay(1000);
}

void updateGPSLocation()
{
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
      Serial.println("GPS Module is not available");
    }
  }
  gpsObject.get_position(&lat, &lon, &age);
  gpsObject.f_get_position(&flat, &flon, &age);
  Serial.print("GPS Module got coordinates"); Serial.print(lat);Serial.print(" ");Serial.println(lon);Serial.print(flat);Serial.print(" ");Serial.println(flon);
  return;
}

void loop() // run over and over
{
  updateGPSLocation();
  unsigned long startTime = millis();
  while (millis() - startTime < 3000) 
  {
    
  }
}

void gpsdump(TinyGPS &gps)
{
  long lat, lon;
  float flat, flon;
  unsigned long age, date, time, chars;
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned short sentences, failed;
  gps.get_position(&lat, &lon, &age);
  gps.f_get_position(&flat, &flon, &age);
  Serial.print("I'm in danger please find me here http://maps.google.com/maps?q=loc:");printFloat(flat, 5); Serial.print(","); printFloat(flon, 5);
  gps.get_datetime(&date, &time, &age);
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  gps.stats(&chars, &sentences, &failed);
}

void printFloat(double number, int digits)
{
  // Handle negative numbers
  if (number < 0.0) 
  {
     Serial.print('-');
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;
  
  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  Serial.print(int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0)
    Serial.print("."); 

  // Extract digits from the remainder one at a time
  while (digits-- > 0) 
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    Serial.print(toPrint);
    remainder -= toPrint;
  }
}
