/*  
 *   First complete sketch for Sonaqua at Dinacon
 *   
 *   by Scott Kildall
 *   www.kildall.com
 *   
 *   Uses the Mozzi Libraries
 *   
 *   We use a combination of an aPhasor and a cosine wave
 *   which gets modulated to the audio synth.
 * -----------------------------------------  
 *   Sonaqua is copyright
 *   Scott Kildall, 2018, CC by-nc-sa.  
 * -----------------------------------------
 *   Mozzy is copyright
 *   Tim Barrass 2012, CC by-nc-sa.
 * -----------------------------------------
*/

#include <MozziGuts.h>
#include <Oscil.h>
#include <Phasor.h>
#include <tables/cos2048_int8.h> // table for Oscils to play
#include <mozzi_midi.h> // for mtof
#include <mozzi_fixmath.h>

#define CONTROL_RATE 64 // powers of 2 please

Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aCos(COS2048_DATA);
Phasor <AUDIO_RATE> aPhasor;


//-- MOZZY CONTROLS
float minAPhasor = 10.f;
float maxAPhasor = 800.f;
float curFreq  = minAPhasor;

//-- SONAQUA STUFF
#define ECPower (A2)
#define ECPin (A1) 

//-- power led
#define LEDPin (12)

// comment out for no serial debug (much faster)
//#define SERIAL_DEBUG

int minEC = 0;
int maxEC = 1000; 
unsigned int curEC;

void setup(){
  randomSeed(A0);  

  #ifdef SERIAL_DEBUG
    Serial.begin(115200);
    Serial.println("Starting up");
  #endif
    
  initSonaqua();
  initMozzy();
}

void initSonaqua() {
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
}

void initMozzy() {
  aCos.setFreq(mtof(curFreq/10));
  aPhasor.setFreq(curFreq);
  
  startMozzi(CONTROL_RATE);
}

void updateControl(){
  curEC = getEC(); 
  curEC = 500 + random(5);
  
  //-- map rawEC, which is in range 0-1023 to freq range
  curFreq = (float)map(curEC, maxEC, minEC, (int)minAPhasor, (int)maxAPhasor);
  
  aCos.setFreq(mtof(curFreq));
  aPhasor.setFreq(curFreq);
}

//-- Sample EC using Analog pins, returns 0-1023
unsigned int getEC(){
  unsigned int raw;
 
  digitalWrite(ECPower,HIGH);

  // This is not a mistake, First reading will be low beause of charged a capacitor
  raw = analogRead(ECPin);
  raw = analogRead(ECPin);   
  
  digitalWrite(ECPower,LOW);
 
 return raw;
}

int updateAudio(){
  if( curEC > maxEC )
    return 0;
    
  char asig1 = (char)(aPhasor.next()>>24);

   return aCos.next()/(asig1) + random(5 + curFreq/20);
}

void loop(){
  audioHook();
}
