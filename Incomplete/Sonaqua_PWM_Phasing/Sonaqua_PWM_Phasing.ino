/*  Example of pulse width modulation,
    using Mozzi sonification library.
     
    Based Miller Puckette's j03.pulse.width.mod example 
    in the Pure Data documentation, subtracting 2 sawtooth 
    waves with slightly different tunings to produce a 
    varying phase difference.
  
    Demonstrates Phasor().
  
    Circuit: Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or 
    check the README or http://sensorium.github.com/Mozzi/
  
    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users
  
    Tim Barrass 2012, CC by-nc-sa.
*/

#include <MozziGuts.h>
#include <Phasor.h>

#define CONTROL_RATE 64 // powers of 2 please

Phasor <AUDIO_RATE> aPhasor1;
Phasor <AUDIO_RATE> aPhasor2;

float freq = 60.f;


//-- sonaqua stuff
#define ECPower (A2)
#define ECPin (A1) 

//-- power led
#define LEDPin (12)

void setup(){
  randomSeed(A0);
  
   //-- pin inputs / outputs
  pinMode(ECPin,INPUT);
  pinMode(ECPower,OUTPUT);                // set pin for sourcing current


  pinMode(LEDPin,OUTPUT);                // set pin for sourcing current

  // Flash LED
  for(int i = 0; i < 6; i++ ) {
    digitalWrite(LEDPin,HIGH);
    delay(100);
    digitalWrite(LEDPin,LOW);
    delay(100);
  }
  digitalWrite(LEDPin,HIGH);

  aPhasor2.setFreq(freq);  
  startMozzi(CONTROL_RATE); // set a control rate of 64 (powers of 2 please)
}


void updateControl(){
  aPhasor1.setFreq((float)(getEC()- 100 + random(30)));
  aPhasor1.setFreq((float)(freq  + random(getEC()/10)));
  
  unsigned int rawEC = getEC(); 
}


int updateAudio(){
  if( getEC() > 1000 )
    return 0;
    
  char asig1 = (char)(aPhasor1.next()>>24);
  char asig2 = (char)(aPhasor2.next()>>24);

  // overweight?
  return ((int)(asig1-asig2))/2;

  
}


void loop(){
  
    
  audioHook(); // required here
}


//-- Sample EC using Analog pins, returns 0-1023
unsigned int getEC(){
  return 500 + random(10);
  
  unsigned int raw;
 
  digitalWrite(ECPower,HIGH);

  // This is not a mistake, First reading will be low beause of charged a capacitor
  raw= analogRead(ECPin);
  raw= analogRead(ECPin);   
  
  digitalWrite(ECPower,LOW);
 
 return raw;
}




