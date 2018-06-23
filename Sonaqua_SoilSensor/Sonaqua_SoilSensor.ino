/*
 *  Sonaqua_SoilSensor
 *  by Scott Kildall
 *  www.kildall.com
 *  
 *  Sonaqua: Uses an analog soil sensor (Soil Moisture Sensor) to make sounds.
 *  
 *  Run sensor through A1, but can be any analog
 *  
 *  Code needs some cleaning
 *  
 */

#include "MSTimer.h"

//-- leave uncommented to see serial debug messages
#define SERIAL_DEBUG

//-- Conditional defines for testing hardware
//#define SPEAKER_ONLY            // just play ascending tones, to make sure speaker works, tests speakers
//  #define SPEAKER_POT             // plays speaker according to potentiometer, tests pot

//-- Conditional to modulate a range on the pot
//#define POT_RANGE_TEST


//-- PINS
#define speakerPin (9)
#define waterLEDPin (10)
#define LEDPin (8)

#define SwitchPin (7)


#define PotPin (A0)
#define ECPower (A2)
#define ECPin (A1)   

#define HeartRateInputPin (A1)   

//-- NeoPixel colors
int r;
int b;
int g;

unsigned int toneValue;

//-- If raw EC is above this, we won't play the sounds
#define EC_SILENT_THRESHOLD (970)
#define DELAY_TIME (100)
#define MIN_TONE (60)   // lowest possible tone for Arduinos
#define MIN_TONE_THRESHOLD (60)   // Under this and we drop it down a bit more

MSTimer displayTimer = MSTimer();

//-- if this is set to TRUE, then we look at a pin for a digital input (from another Arduino), which acts like a switch
//-- OR this can be just a switch to activate
boolean bUseSwitch = false;


//-- heartbeat stuff
int Threshold = 700;            // Determine which Signal to "count as a beat", and which to ingore. 
int Signal;


void setup() {
  
   //-- pin inputs / outputs
  pinMode(ECPin,INPUT);
  pinMode(ECPower,OUTPUT);                // set pin for sourcing current
  pinMode(speakerPin, OUTPUT); 
  pinMode(LEDPin,OUTPUT);                // set pin for sourcing current
  pinMode(waterLEDPin,OUTPUT);
  
  pinMode(SwitchPin, INPUT);


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

 
  //-- speaker ground is always low
  //digitalWrite(speakerGndPin,LOW);

  // every 1000ms we will update the display, for readability
  displayTimer.setTimer(1000);
  displayTimer.start();
   
  
  digitalWrite(waterLEDPin, LOW);
}

//-- rawEC == 0 -> max conductivity; rawEC == 1023, no conductivity
void loop() { 
  
  Signal = analogRead(HeartRateInputPin);  // Read the PulseSensor's value. 
  Serial.println(Signal);

  if( Signal < 10 )
    noTone(speakerPin);
  else  
    tone(speakerPin, Signal/5 );
  delay(100);
  return;
}





