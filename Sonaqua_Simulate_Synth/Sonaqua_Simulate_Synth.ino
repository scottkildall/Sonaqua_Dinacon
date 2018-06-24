/*  
 *   Sonaqua_Phasemod_Humidity: Sketch for Dinacon
 *   
 *   by Scott Kildall
 *   www.kildall.com
 *   
 *   Uses the Mozzi Libraries
 *   
 *   Based on the Phasemod_Enveloipe example
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
#include <tables/cos8192_int8.h>
#include <tables/envelop2048_uint8.h>

//-- SIMULATION 1
#define CONTROL_RATE 512 // quite fast, keeps modulation smooth

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <COS8192_NUM_CELLS, AUDIO_RATE> aCarrier(COS8192_DATA);
Oscil <COS8192_NUM_CELLS, AUDIO_RATE> aModulator(COS8192_DATA);
Oscil <COS8192_NUM_CELLS, AUDIO_RATE> aModWidth(COS8192_DATA);
Oscil <COS8192_NUM_CELLS, CONTROL_RATE> kModFreq1(COS8192_DATA);
Oscil <COS8192_NUM_CELLS, CONTROL_RATE> kModFreq2(COS8192_DATA);
Oscil <ENVELOP2048_NUM_CELLS, AUDIO_RATE> aEnvelop(ENVELOP2048_DATA);


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

  
  startMozzi(CONTROL_RATE);

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
  //-- initial values
  aCarrier.setFreq(simValue);
  kModFreq1.setFreq(.85f);
  kModFreq2.setFreq(0.1757f);
  aModWidth.setFreq(0.1434f);

  //-- the lower the envelope, the longer the reverb/echo effcts
  aEnvelop.setFreq(8.0f);
}

void updateControl_01() {
    aModulator.setFreq(aModulatorBaseFreq + 0.4313f*kModFreq1.next() + kModFreq2.next());

    kModFreq1.setFreq(.85f + (float)(simValue+random(50))/300.f);
}

int updateAudio_01() {
   aModulatorBaseFreq = (float)simValue/4.0f;

   //-- a short delay does a really crazy schmear of the sounds — the higher the delay, the more it sounds
   //-- like a unified sound as opposed to a series of distinct beats
   //-- the randomness adds a tinny sound at the end
  delayMicroseconds(simValue + random(200) - 100);

   aCarrier.setFreq((float)simValue/6.0f);

  int asig = aCarrier.phMod((int)aModulator.next()*(260u+aModWidth.next()));
  return (asig*(byte)aEnvelop.next())>>8;
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


