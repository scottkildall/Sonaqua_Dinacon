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
long control_steps_per_gliss = CONTROL_RATE/8;      // a lower number makes for a speedier gliss

// stuff for changing starting positions, probably just confusing really
int counter = 0;
byte gliss_offset = 0;
byte gliss_offset_step = 2;
byte  gliss_offset_max = 36;

bool glissOn = false;

int aCounter= 0;
int glissModulo = 40;
int lightInput;

MSTimer glissTimer;
MSTimer lightReadTimer;

void setup(){
  Serial.begin(115200);
  Serial.println("Starting up: S_");
  
  kDelay.set(echo_cells_1);
  startMozzi();

  Serial.print("cs:" );
  Serial.println( control_steps_per_gliss );
     
  glissTimer.setTimer(10000);
  lightReadTimer.setTimer(1000);

  Serial.println(getLightInput());
  Serial.println(lightInput);
}


void updateControl(){
  //-- the read call seems to adjust the values too much, resulting in an odd effect, tie these to a timer?
 // int mozziAnalogRead(lightSensorPin);
 // Serial.println(lightInput);
 
  Serial.println(getLightInput());
  
 glissModulo = 40 + (lightInput >> 4);

 // Serial.println(glissModulo);

 // Serial.println(lightInput);
  
  averaged = kAverage.next(lightInput);

  float lowFreqMult = 1.5f;

  lowFreqMult = .5f + (float)(random(10,20)/10);
  
  //-- divide the averages to lower the frequency
  aSin0.setFreq(averaged/lowFreqMult + random(100));                 // this random effect is significant, has nice effects, like bubbling water
  aSin1.setFreq(kDelay.next(averaged)/4.0f + random(10));          // also significant, but not desirable
  aSin2.setFreq(kDelay.read(echo_cells_2) / 8.0f + random(10));        // don't like this high-value, random effect         
  aSin3.setFreq(kDelay.read(echo_cells_3)/ 12.0f + random(300));      // this random effect not-so signficant, but good for slight modulations

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

  if( random(300) == 0 ) {
     if( control_steps_per_gliss == 16 )
       control_steps_per_gliss = 8;
     else  
       control_steps_per_gliss = 16;

     Serial.print("swapping cs:" );
     Serial.println( control_steps_per_gliss );
  }
  
  if( glissOn == false && random(500) == 0 ) {
    glissOn = true;
    glissTimer.setTimer(random(5000, 8000));
  }
  
  if( glissOn && glissTimer.isExpired() )
    glissOn = false;
}


int updateAudio(){
  aCounter++;

  /*
  int audioVal= 3*((int)aSin0.next()+aSin1.next()+(aSin2.next()>>1)
    +(aSin3.next()>>2)) >>3;
*/

   int audioVal;

  //-- pull some out
  /*if( (random(2)) == 0 )
     audioVal = 2*((int)aSin0.next()+(aSin2.next()>>1)
      +(aSin3.next()>>2)) >>3;
   else
*/
       audioVal= 2*((int)aSin0.next()+aSin1.next()+(aSin2.next()>>1)
    +(aSin3.next()>>2)) >>3;
    
  if( glissOn ) {
      //-- this modulo gives a nice high freq spinny effect, to counter the lower
     if( (aCounter % glissModulo) == 0 ) {
      aTriangle.setPhaseInc(aGliss.next());
      audioVal += (aTriangle.next()/4);
    }
  }

//    Serial.println(audioVal);
  
    //-- brute slowdown to create a weird effect
    //delayMicroseconds(10);

    return (char)audioVal;
}

int getLightInput() {
  lightInput = mozziAnalogRead(lightSensorPin);
  Serial.println(lightInput);

  lightInput = map(lightInput,0,1023,200,900);
  return lightInput;
}

void loop(){
  audioHook();
}


