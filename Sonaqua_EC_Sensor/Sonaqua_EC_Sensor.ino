/*
 *  Sonaqua_SoilSensor
 *  by Scott Kildall
 *  www.kildall.com
 *  
 *  Sonaqua: Uses two analog soil sensor (Soil Moisture Sensor) to make sounds.
 *  This will be so that plants can "talk" to each other
 *  
 *  Derivation fromthe Light_Temperature_Multi_Oscil example
 *  
 */

#include "MSTimer.h"

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/cos8192_int8.h>
#include <mozzi_midi.h>

#define CONTROL_RATE 256

#define THERMISTOR_PIN 1
#define LDR_PIN 2
#define NUM_VOICES 8
#define THRESHOLD 10

// harmonics
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos1(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos2(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos3(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos4(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos5(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos6(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos7(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos0(COS8192_DATA);

// volume controls
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol1(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol2(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol3(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol4(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol5(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol6(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol7(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol0(COS8192_DATA);

// audio volumes updated each control interrupt and reused in audio till next control
char v1,v2,v3,v4,v5,v6,v7,v0;

// notes to play depending on whether temperature reading increases or decreases
float upnotes[NUM_VOICES] = {
  mtof(32.f),mtof(34.f),mtof(40.f),mtof(42.f),mtof(49.f),mtof(51.f), mtof(56.f), mtof(58.f)};

float downnotes[NUM_VOICES] = {
  mtof(64.f),mtof(65.f),mtof(88.f),mtof(72.f),mtof(79.f),mtof(84.f),mtof(86.f),mtof(89.f)};

  
//-- leave uncommented to see serial debug messages
#define SERIAL_DEBUG


//-- PINS
#define speakerPin (9)
#define blueLEDPin (10)
#define greenLEDPin (8)

#define ECPower (A2)
#define ECPin (A1) 
  
int signal1;


int previousSoilReading = 0;

MSTimer differentialTimer;
float differentialFreq = 5.0;

void setup() {
  randomSeed(A2);
#ifdef SERIAL_DEBUG
  //-- no serial support for the Digispark
  Serial.begin(115200);
  Serial.println("startup");
#endif

  initLED();
  pinMode(speakerPin, OUTPUT); 
  differentialTimer.setTimer(10000);
  differentialFreq = (float)5.0;
  
  startMozzi(CONTROL_RATE);
}

//-- Flash LED a few times
void initLED() {
  pinMode(greenLEDPin,OUTPUT);   
  pinMode(blueLEDPin,OUTPUT);   
  pinMode(ECPin,INPUT);
  pinMode(ECPower,OUTPUT);                // set pin for sourcing current

  
  // Flash LED
  for(int i = 0; i < 6; i++ ) {
    digitalWrite(greenLEDPin,HIGH);
    digitalWrite(blueLEDPin,HIGH);
    delay(100);
    digitalWrite(greenLEDPin,LOW);
    digitalWrite(blueLEDPin,LOW);
    delay(100);
  }
  
  digitalWrite(greenLEDPin,HIGH);
  digitalWrite(blueLEDPin,HIGH);

//-- inital readings
  readSoil();
  
  Serial.print("analog signal = ");
  Serial.println(signal1);
}

//-- rawEC == 0 -> max conductivity; rawEC == 1023, no conductivity
void loop() { 
  
  audioHook();

  return;
}

// returns freq
int soilToFreq(char oscil_num, int soilReading){
 // Serial.println(soilReading);
  int freq;
  if (soilReading>previousSoilReading){
    // Serial.println("up");
    freq = upnotes[oscil_num];
  } else {
    //Serial.println("down");
     freq = downnotes[oscil_num];
  }
  
  previousSoilReading = soilReading;
  return freq;
}

void readSoil() {
  //signal1 = 650 + random(5);

//-- the non-simulator doesn't sound as amazing as the simulator!
  signal1 = getEC();
  //Serial.println(signal1);
  /*
  int signal1 = mozziAnalogRead(soilSensorPin1); // not calibrated to degrees!
  int signal2 = mozziAnalogRead(soilSensorPin1);
  */

}
void updateControl(){
  static float previous_pulse_freq;
    
  // read analog inputs
  readSoil();

  // do somthing with this — the lower the differential, the slow the waves
  // 5f is super slow (and nice)
  // 100f is too 

  if( differentialTimer.isExpired() ) {
    differentialFreq = (float)random(5,20);
    differentialTimer.start();
  }
  
  float differential = differentialFreq + random(10);
  
  // map light reading to volume pulse frequency
  float pulse_freq = differential/256.0f;
  previous_pulse_freq = pulse_freq;

  v0 = kVol0.next();
  v1 = kVol1.next();
  v2 = kVol2.next();
  v3 = kVol3.next();
  v4 = kVol4.next();
  v5 = kVol5.next();
  v6 = kVol6.next();
  v7 = kVol7.next();
  
  // set one note oscillator frequency each time (if it's volume is close to 0)
  static char whoseTurn;

  float soilReading = signal1;
  
  switch(whoseTurn){  
  case 0:
    kVol0.setFreq(pulse_freq);
    if(abs(v0)<THRESHOLD) aCos0.setFreq(soilToFreq(0,soilReading));
    break;

  case 1:
    kVol1.setFreq(pulse_freq);
    if(abs(v1)<THRESHOLD) aCos1.setFreq(soilToFreq(1,soilReading));
    break;

  case 2:
    kVol2.setFreq(pulse_freq);
    if(abs(v2)<THRESHOLD) aCos2.setFreq(soilToFreq(2,soilReading));
    break;

  case 3:
    kVol3.setFreq(pulse_freq);
    if(abs(v3)<THRESHOLD) aCos3.setFreq(soilToFreq(3,soilReading));
    break;

  case 4:
    kVol4.setFreq(pulse_freq);
    if(abs(v4)<THRESHOLD) aCos4.setFreq(soilToFreq(4,soilReading));
    break;

  case 5:
    kVol5.setFreq(pulse_freq);
    if(abs(v5)<THRESHOLD) aCos5.setFreq(soilToFreq(5,soilReading));
    break;

  case 6:
    kVol6.setFreq(pulse_freq);
    if(abs(v6)<THRESHOLD) aCos6.setFreq(soilToFreq(6,soilReading));
    break;

  case 7:
    kVol7.setFreq(pulse_freq);
    if(abs(v7)<THRESHOLD) aCos7.setFreq(soilToFreq(7,soilReading));
    break;

  }

  if(++whoseTurn>=NUM_VOICES) whoseTurn = 0;
}


int updateAudio(){
  
  if( signal1 > 1000 )
      return 0;
      
  long asig = (long)
    aCos0.next()*v0 +
      aCos1.next()*v1 +
      aCos2.next()*v2 +
      aCos3.next()*v3 +
      aCos4.next()*v4;
      asig >>= 9; // shift back to audio output range
 
      /*aCos5.next()*v5 +
      aCos6.next()*v6 +
      aCos7.next()*v7;
      */
  return (int) asig;
}

//-- Sample EC using Analog pins, returns 0-1023
unsigned int getEC(){
  unsigned int raw;
 
  digitalWrite(ECPower,HIGH);

  // This is not a mistake, First reading will be low beause of charged a capacitor
  raw= analogRead(ECPin);
  raw= analogRead(ECPin);   
  
  digitalWrite(ECPower,LOW);
  

 return raw;
}
