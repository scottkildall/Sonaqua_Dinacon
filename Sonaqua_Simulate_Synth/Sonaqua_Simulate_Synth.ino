/*  
 *   Sonaqua_Simulate_Synth: Scratch Sketch for simulating audio synth
 *   This is always a work-in-progress, used to for developing soundscapes before putting
 *   in actual sensor code.
 *   
 *   by Scott Kildall
 *   www.kildall.com
 *   
 *   Uses the Mozzi Libraries
 *   
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

  switch(simNumber) {
    case 1:
      initSim_01();
      break;
  }

  startMozzi(CONTROL_RATE);
}

int getSimValue() {
  return 700 + random(10);
}

void updateControl() {
  switch(simNumber) {
    simValue = getSimValue();
     
    case 1:
      updateControl_01();
      break;
  }
}

//------ SIMUATION 1 --------
void initSim_01() {
  ratio = float_to_Q8n8(10.0f);   // define modulation ratio in float and convert to fixed-point
  kNoteChangeDelay.set(10000); // note duration ms, within resolution of CONTROL_RATE
  aModDepth.setFreq(1.f);     // vary mod depth to highlight am effects
  randomSeed(A0); // reseed the random generator for different results each time the sketch runs

}

void updateControl_01() {
   static Q16n16 last_note = octave_start_note;

  //aModDepth.setFreq((float)random(100,simValue));

  if( random(5) == 0 )
    aModDepth.setFreq((float)random(100,simValue));
    
  if(kNoteChangeDelay.ready() ) {
    //-- do some random changes to make it feel more dynamic
    kNoteChangeDelay.set(random(5000,10000));
    //aModDepth.setFreq((float)random(1,30));
  
    // change octave now and then
 //   if(rand((byte)5)==0){

    // the 3 value will keep it at lower octaves (more pleasant to the ear)
      last_note = 36+(rand((byte)3)*12);

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

int updateAudio_01() {
    long mod = (128u+ aModulator.next()) * ((byte)128+ aModDepth.next());
  int out = (mod * aCarrier.next())>>16;
  return out;
}

//-----------------------------

int updateAudio(){
  switch(simNumber) {
    simValue = getSimValue();
     
    case 1:
      return updateAudio_01();
      break;
  }


  return 0;
}


void loop() {
  audioHook();
}


