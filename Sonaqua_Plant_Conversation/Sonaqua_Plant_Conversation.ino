/*  
 *   Sonaqua_Plant_Conversation:
 *   Two sensors hooked up to two different plants
 *   that will simulate a conversation they might have
 *   
 *   by Scott Kildall
 *   www.kildall.com
 *   
 *   Uses the Mozzi Libraries
 *   
 *   Based on the Based on the AM_Synthesis example example
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
#include <tables/cos2048_int8.h> // table for Oscils to play
//#include <mozzi_utils.h>
#include <mozzi_fixmath.h>
#include <EventDelay.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <Phasor.h>


#define CONTROL_RATE 64 // powers of 2 please

// audio oscils
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aCarrier(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aModulator(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aModDepth(COS2048_DATA);

// for scheduling note changes in updateControl()
EventDelay  kNoteChangeDelay;

// synthesis parameters in fixed point formats
Q8n8 ratio; // unsigned int with 8 integer bits and 8 fractional bits
Q24n8 carrier_freq; // unsigned long with 24 integer bits and 8 fractional bits
Q24n8 mod_freq; // unsigned long with 24 integer bits and 8 fractional bits

// for random notes
Q8n0 octave_start_note = 42;


//-- simulated EC level
int simEC = 300;

//-- SONAQUA STUFF


//-- power led
#define LEDPin (12)

// comment out for no serial debug (much faster)
#define SERIAL_DEBUG

int simValue;
int simNumber = 1;

boolean sim1 = true;

//--
float aModulatorBaseFreq = 135.0f;


void setup() {
  #ifdef SERIAL_DEBUG
    Serial.begin(115200);
    Serial.println("Starting up: Sonaqua_Phasemod_Humidity");
  #endif
  
  
 // Flash LED
   pinMode(LEDPin,OUTPUT);                // set pin for sourcing current

  for(int i = 0; i < 6; i++ ) {
    digitalWrite(LEDPin,HIGH);
    delay(100);
    digitalWrite(LEDPin,LOW);
    delay(100);
  }
  digitalWrite(LEDPin,HIGH);


  

  simValue = getSimValue();
  
 #ifdef SERIAL_DEBUG
  Serial.print("initial humidity value = ");
  Serial.println(simValue);
#endif

 
  initMozzy();
  
 
}

int getSimValue() {
  if( sim1 )
    return 700 + random(10);
 else
  return 500 + random(10);
}


void initMozzy() {
  ratio = float_to_Q8n8(10.0f);   // define modulation ratio in float and convert to fixed-point
  kNoteChangeDelay.set(10000); // note duration ms, within resolution of CONTROL_RATE
  aModDepth.setFreq(1.f);     // vary mod depth to highlight am effects
  randomSeed(A0); // reseed the random generator for different results each time the sketch runs

   startMozzi(CONTROL_RATE);
}

void updateControl() {
   static Q16n16 last_note = octave_start_note;
   simValue = getSimValue();
   
    //-- frequency is the basic oscilator
   if( random(2) == 0 )
    aModDepth.setFreq((float)simValue + random(250));

  //-- this creates what appears to be a syncopatic feel to it — the lower the random number, the more glitchy it sounds

  if( random(10) == 0 )
    //-- this sets a much higher pitch — will sound glitchy
    aCarrier.setFreq_Q24n8(carrier_freq * ((float)random(10,50)));
  else
    aCarrier.setFreq_Q24n8(carrier_freq);
    
  if(kNoteChangeDelay.ready() ) {
      // flip between the 2 plants
     sim1 = !sim1;
     
    //-- do some random changes to make it feel more dynamic
    kNoteChangeDelay.set(random(3000,10000));
    
    
    // the value after (byte) will keep it at lower octaves (more pleasant to the ear)
      last_note = 36+(rand((byte)2)*12);

       // change step up or down a semitone, randomly
      if( random(2) == 0 )
        last_note += 1-rand((byte)3);
      else
        last_note += 3 * (1-rand((byte)3));
    
       // change modulation ratio now and then
       ratio = ((Q8n8) 1+ rand((byte)5)) <<8;
       ratio += rand((byte)255);

    // convert midi to frequency
    Q16n16 midi_note = Q8n0_to_Q16n16(last_note); 
    carrier_freq = Q16n16_to_Q24n8(Q16n16_mtof(midi_note));

    // calculate modulation frequency to stay in ratio with carrier
    mod_freq = (carrier_freq * ratio)>>8; // (Q24n8   Q8n8) >> 8 = Q24n8

    // set frequencies of the oscillators
    aCarrier.setFreq_Q24n8(carrier_freq);
    aModulator.setFreq_Q24n8(mod_freq);

    // reset the note scheduler
    kNoteChangeDelay.start();
  }
}

int updateAudio() {
  long mod = (128u+ aModulator.next()) * ((byte)128+ aModDepth.next());
  int out = (mod * aCarrier.next())>>16;
  return out;
}


void loop() {
  audioHook();
}


