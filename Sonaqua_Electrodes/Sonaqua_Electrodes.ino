/*
 *  Sonaqua_Electrodes
 *  by Scott Kildall
 *  www.kildall.com
 *  
 *  Sonaqua: Use electrode pads.
 *  
 *  1K resistor on the other side
 *  
 */

#include "MSTimer.h"

//-- leave uncommented to see serial debug messages
#define SERIAL_DEBUG

//-- PINS
#define speakerPin (9)
#define LEDPin (12)

#define ECPower (A2)
#define electrodePin (A1)   



unsigned int toneValue;

//-- If raw EC is above this, we won't play the sounds

MSTimer displayTimer = MSTimer();

//-- if this is set to TRUE, then we look at a pin for a digital input (from another Arduino), which acts like a switch
//-- OR this can be just a switch to activate
boolean bUseSwitch = false;


//-- heartbeat stuff
int Threshold = 700;            // Determine which Signal to "count as a beat", and which to ingore. 
int Signal;

#define SERIAL_DEBUG

void setup() {
  
   //-- pin inputs / outputs
  pinMode(electrodePin,INPUT);
  pinMode(ECPower,OUTPUT);                // set pin for sourcing current
  pinMode(speakerPin, OUTPUT); 
  pinMode(LEDPin,OUTPUT);                // set pin for sourcing current
 
  // Flash LED
  for(int i = 0; i < 6; i++ ) {
    digitalWrite(LEDPin,HIGH);
    delay(100);
    digitalWrite(LEDPin,LOW);
    delay(100);
  }
  digitalWrite(LEDPin,HIGH);

  
#ifdef SERIAL_DEBUG
  //-- no serial support for the Digispark
  Serial.begin(115200);
  Serial.println("startup");
#endif
}

//-- rawEC == 0 -> max conductivity; rawEC == 1023, no conductivity
void loop() { 
  int ec = getEC();

 #ifdef SERIAL_DEBUG
  Serial.println(ec);
 #endif

  if( ec < 1000 )
    tone(speakerPin, ec/5 + random(5));
  else 
    noTone(speakerPin);

   delay(100);

  return;
}


//-- Sample EC using Analog pins, returns 0-1023
unsigned int getEC(){
  unsigned int raw;
 
  digitalWrite(ECPower,HIGH);

  // This is not a mistake, First reading will be low beause of charged a capacitor
  raw= analogRead(electrodePin);
  raw= analogRead(electrodePin);   
  
  digitalWrite(ECPower,LOW);
  

 return raw;
}




