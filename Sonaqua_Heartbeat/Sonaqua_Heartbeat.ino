/*
 *  Sonaqua_Heartbeat
 *  by Scott Kildall
 *  www.kildall.com
 *  
 *  Sonaqua: Uses a Pulserate HR sensor to make sounds. The tones get modulated from the HR sensor.
 *  The higher the beats-per-minute, the higher the tone.
 *  
 *  This uses the Sonaqa circuit boards (without probe leads).
 *  
 *  Run sensor through SDA/A4, so you can easilyput in the leads.
 *  
 *  Full project description here:
 *  https://kildall.com/project/sonaqua/
 *  
 *  Needs some fixing on the BMP calculations. If I wanted to improve this sketch, I'd do a calc of bmp
 *  for the last 10 beats (or some other number).
 *  
 */
 
#include "MSTimer.h"

//-- leave uncommented to see serial debug messages
#define SERIAL_DEBUG


//-- PINS
#define speakerPin  (9)
#define hrLEDPIN (8)
#define powerLEDPin (12)

#define ECPower (A2)            // holdover from Sonaqua, not sure if this is necessary, but whatevs
#define heartRateInputPin (A1)   

//-- heartbeat stuff
int threshold = 700;            // Determine which threshold to "count as a beat", and which to ingore. 
int hrSignal;
unsigned long bmp;  // beats per min


MSTimer aboveThresholdTimer;
unsigned long beatsStartTime;
unsigned long numBeats;
boolean beatOn = false;
boolean bAboveThreshold = false;

void setup() {
//-- serial debugging
#ifdef SERIAL_DEBUG
  Serial.begin(115200);
  Serial.println("startup");
#endif

 //-- pin inputs / outputs
  pinMode(heartRateInputPin,INPUT);
  pinMode(ECPower,OUTPUT);                // set pin for sourcing current
  pinMode(speakerPin, OUTPUT); 
  pinMode(powerLEDPin,OUTPUT);                // set pin for sourcing current
  pinMode(hrLEDPIN,OUTPUT);


// Flash LEDs upon startup, then turn power LED on and HR indicator off
  for(int i = 0; i < 6; i++ ) {
    digitalWrite(powerLEDPin,HIGH);
    digitalWrite(hrLEDPIN,HIGH);
    delay(100);
    digitalWrite(powerLEDPin,LOW);
    digitalWrite(hrLEDPIN,LOW);
    delay(100);
  }
  digitalWrite(powerLEDPin,HIGH);
  digitalWrite(hrLEDPIN, LOW);

  aboveThresholdTimer.setTimer(2000);
}

void loop() { 
  hrSignal = analogRead(heartRateInputPin);  // Read the PulseSensor's value. 
  
  if(hrSignal > threshold)   {  
    if( bAboveThreshold == false ) {
      aboveThresholdTimer.start();
      bAboveThreshold = true;
    }
    else if( aboveThresholdTimer.isExpired() ) {
#ifdef SERIAL_DEBUG
      Serial.println("---");
#endif
      noTone(speakerPin);
      numBeats = 0;
    }
    
    if( beatOn == false)  {             
      tone(speakerPin,bmp);
      digitalWrite(hrLEDPIN, HIGH);

#ifdef SERIAL_DEBUG
      Serial.println("BEAT");
      Serial.print("bmp =");
      Serial.println( bmp );
#endif      
      numBeats++;

      if( numBeats == 1 )
        beatsStartTime = millis();

      if( numBeats > 1 ) {
        //-- bmp calc, though I think it needs to be fixed up
        float minutes = (float)(millis() - beatsStartTime)/60000.0f;
        bmp =  (unsigned long)((float)numBeats/minutes);
     }

      beatOn = true;
    }
  }
 else {
    if( bAboveThreshold == true )
      bAboveThreshold = false;
      
    if( beatOn == true ) {
       noTone(speakerPin);
      digitalWrite(hrLEDPIN, LOW);

#ifdef SERIAL_DEBUG
       Serial.println("NO BEAT");
#endif
    }   
    beatOn = false; 
   }

  delay(2);
  return;
}




