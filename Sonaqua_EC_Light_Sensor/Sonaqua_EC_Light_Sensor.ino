/*  
    Sonaqua_EC_Light_Sensor
    by Scott Kildall

    Adapted from the Mozzy examples:
    Control_Echo_Theremin and Line_Gliss
    
 * -----------------------------------------  
 *   Sonaqua is copyright
 *   Scott Kildall, 2018, CC by-nc-sa.  
 * -----------------------------------------
 *   Mozzy is copyright
 *   Tim Barrass 2012, CC by-nc-sa.
*/

#include "MSTimer.h"

#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <RollingAverage.h>
#include <ControlDelay.h>

#include <Line.h> // for smooth transitions
#include <tables/triangle_warm8192_int8.h> // triangle table for oscillator
#include <mozzi_midi.h>

#define lightSensorPin A4    // analog control input
#define ECPower (A2)
#define ECPin (A1)   

unsigned int echo_cells_1 = 32;   // 32
unsigned int echo_cells_2 = 60;   // 60
unsigned int echo_cells_3 = 127; //127;

#define CONTROL_RATE 64
ControlDelay <128, int> kDelay; // 2seconds

// oscils to compare bumpy to averaged control input
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin0(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin1(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin2(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin3(SIN2048_DATA);

// use: RollingAverage <number_type, how_many_to_average> myThing
RollingAverage <int, 32> kAverage; // how_many_to_average has to be power of 2
int averaged;


//--- LINE Gliss

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> aTriangle(TRIANGLE_WARM8192_DATA);

// use: Line <type> lineName
Line <long> aGliss;

#define CONTROL_RATE 64 // powers of 2 please

byte lo_note = 24; // midi note numbers
byte hi_note = 36;

long audio_steps_per_gliss = AUDIO_RATE / 4;      // ie. 4 glisses per second
long control_steps_per_gliss = CONTROL_RATE/4;      // a lower number makes for a speedier gliss

// stuff for changing starting positions, probably just confusing really
int counter = 0;
byte gliss_offset = 0;
byte gliss_offset_step = 2;
byte  gliss_offset_max = 36;

bool glissOn = false;

int aCounter= 0;

//-- gliss stuff
int glissModulo = 20;
int glissMin = 20;
int glissMax = 80;

int lightInput;
unsigned long rawEC;

MSTimer glissTimer;
MSTimer lightReadTimer;

#include <mozzi_rand.h>

void setup(){
  randomSeed(A0);
  //randSeed(); // reseed the random generator for different results each time the sketch runs
  
   pinMode(ECPin,INPUT);
  pinMode(ECPower,OUTPUT);                // set pin for sourcing current


  Serial.begin(115200);
  Serial.println("Starting up: S_");
  
  kDelay.set(echo_cells_1);
  startMozzi();

  Serial.print("cs:" );
  Serial.println( control_steps_per_gliss );
     
  glissTimer.setTimer(5000);
  lightReadTimer.setTimer(1000);

  Serial.println(getLightInput());
  Serial.println(lightInput);
}


void updateControl(){
  //-- the read call seems to adjust the values too much, resulting in an odd effect, tie these to a timer?
 
 // Serial.println(getLightInput());

  getLightInput();
  rawEC = getEC();
 // Serial.println(rawEC);
  
// glissModulo = 40 + (lightInput >> 4);

 // Serial.println(glissModulo);

 // Serial.println(lightInput);
  
  averaged = kAverage.next(lightInput);

  float lowFreqMult = 1.5f;

  lowFreqMult = .5f + (float)(random(10,20)/10);
  
  //-- divide the averages to lower the frequency
  aSin0.setFreq(averaged/lowFreqMult + random(20));                 // this random effect is significant, has nice effects, like bubbling water
  aSin1.setFreq(kDelay.next(averaged)/4.0f + random(2));          // also significant, but not desirable
  aSin2.setFreq(kDelay.read(echo_cells_2) / 8.0f + random(2));        // don't like this high-value, random effect         
  aSin3.setFreq(kDelay.read(echo_cells_3)/ 12.0f + random(2));      // this random effect not-so signficant, but good for slight modulations

  lineGliss();

}


void lineGliss() {
  //-- line gliss
  if (--counter <= 0){
    
    // start at a new note
    gliss_offset += gliss_offset_step;
    if(gliss_offset >= gliss_offset_max) gliss_offset=0;
    
    // only need to calculate frequencies once each control update
    int start_freq = mtof(lo_note+gliss_offset);
    int end_freq = mtof(hi_note+gliss_offset);
    
    // find the phase increments (step sizes) through the audio table for those freqs
    // they are big ugly numbers which the oscillator understands but you don't really want to know
    long gliss_start = aTriangle.phaseIncFromFreq(start_freq);
    long gliss_end = aTriangle.phaseIncFromFreq(end_freq);
    
    // set the audio rate line to transition between the different step sizes
    aGliss.set(gliss_start, gliss_end, audio_steps_per_gliss);
    
    counter = control_steps_per_gliss;
  }

  //-- do some reandomization on the control steps, which will result in a longer/slower gliss
//  if( random(300) == 0 ) {
//     if( control_steps_per_gliss == 16 )
//       control_steps_per_gliss = 8;
//     else  
//       control_steps_per_gliss = 16;
//
//     Serial.print("swapping cs:" );
//     Serial.println( control_steps_per_gliss );
//  }
//  
//  if( glissOn == false && random(500) == 0 ) {
//    glissOn = true;
//    glissTimer.setTimer(random(5000, 8000));
//  }
  
  if( glissOn && glissTimer.isExpired() ) {
  
    //glissOn = false;

  
 //   glissModulo += (1 - random(3));

    //-- wider ragnge of randomness for higher values of gliss
    glissModulo = 10 + random(128-(getEC() >> 4));
    
    //-- constraint bounds
    if( glissModulo < glissMin )
      glissModulo  = glissMin;
    else if( glissModulo > glissMax)
      glissModulo = glissMax;
      
    glissTimer.start();
    //Serial.println(glissModulo);
/*
    if( control_steps_per_gliss == 16 )
       control_steps_per_gliss = 8;
     else  
       control_steps_per_gliss = 16;
*/
     Serial.print("swapping cs:" );
     Serial.println( control_steps_per_gliss );
  }

//-- could map gliss modulo here
  //  unsigned long rawEC = getEC()


      
   // glissOn = true;
}

int updateAudio(){
  aCounter++;

  /*
  int audioVal= 3*((int)aSin0.next()+aSin1.next()+(aSin2.next()>>1)
    +(aSin3.next()>>2)) >>3;
*/

   int audioVal;


       audioVal= 1*((int)aSin0.next()+aSin1.next()+(aSin2.next()>>1)
    +(aSin3.next()>>2)) >>3;
    
  if( glissOn ) {
      //-- this modulo gives a nice high freq spinny effect, to counter the lower
     if( (aCounter % glissModulo) == 0 ) {
      aTriangle.setPhaseInc(aGliss.next());
      audioVal += (aTriangle.next()/4);
    }
  }

//    Serial.println(audioVal);
  
    return (char)audioVal;
}

int getLightInput() {
  lightInput = mozziAnalogRead(lightSensorPin);
  //Serial.println(lightInput);

  lightInput = map(lightInput,0,1023,200,900);
  return lightInput;
}

//-- Sample EC using Analog pins, returns 0-1023
unsigned int getEC(){
  unsigned int raw;
 
  digitalWrite(ECPower,HIGH);

  // This is not a mistake, First reading will be low beause of charged a capacitor
  raw = mozziAnalogRead(ECPin);
  raw = mozziAnalogRead(ECPin);   
  
  digitalWrite(ECPower,LOW);
  
  if( raw > 1000 )
    glissOn = false;
  else
    glissOn = true;

  return random(200) + 5;
  
 return raw;
}

void loop(){
  audioHook();
}


