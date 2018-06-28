/*
 *  Sonaqua_Light_Sensor
 *  
 *  by Scott Kildall
 *  www.kildall.com
 *  
 *  Just outputs analog voltages (for now)
 *  
 */

//-- leave uncommented to see serial debug messages
#define SERIAL_DEBUG

//-- PINS
#define speakerPin (9)
#define blueLEDPin (8)
#define greenLEDPin (12)
#define lightPin (A4)   

void setup() {
  pinMode(speakerPin, OUTPUT); 
  pinMode(blueLEDPin,OUTPUT);               
  pinMode(greenLEDPin,OUTPUT);

  // Flash LED
  for(int i = 0; i < 6; i++ ) {
    digitalWrite(greenLEDPin,HIGH);
    digitalWrite(blueLEDPin,HIGH);
    delay(100);
    digitalWrite(greenLEDPin,LOW);
    digitalWrite(blueLEDPin,HIGH);
    delay(100);
  }
  digitalWrite(greenLEDPin,HIGH);
   digitalWrite(blueLEDPin, LOW);
  
#ifdef SERIAL_DEBUG
  //-- no serial support for the Digispark
  Serial.begin(115200);
  Serial.println("startup");
#endif
}

void loop() { 
  Serial.println(analogRead(lightPin));
  return;
 }

