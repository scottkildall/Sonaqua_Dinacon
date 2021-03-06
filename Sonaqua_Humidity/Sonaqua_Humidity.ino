/*  
 *   Sonaqua_Phasemod_Humidity: Sketch for Dinacon
 *   
 *   by Scott Kildall
 *   www.kildall.com
 *   
 *   Uses the Mozzi Libraries
 *   
 *   Based on the Phasemod_Envelope example
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
#define humiditySensorPin (A1) 

//-- power led
#define LEDPin (12)

// comment out for no serial debug (much faster)
#define SERIAL_DEBUG

int minEC = 0;
int maxEC = 1000; 
unsigned int curEC;

int humidityValue;

//--
float aModulatorBaseFreq = 135.0f;

void setup() {
  randomSeed(A0);
  
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

  humidityValue = analogRead(humiditySensorPin);


 #ifdef SERIAL_DEBUG
  Serial.print("initial humidity value = ");
  Serial.println(humidityValue);
#endif

  startMozzi(CONTROL_RATE);

  //-- initial values 
  aCarrier.setFreq(humidityValue);
  kModFreq1.setFreq(.85f);
  kModFreq2.setFreq(0.1757f);
  aModWidth.setFreq(0.1434f);

  //-- the lower the envelope, the longer the reverb/echo effcts
  aEnvelop.setFreq(8.0f);
}


void updateControl() {
 humidityValue = analogRead(humiditySensorPin);

//-- doing this below seems to screw up the timing
/*
#ifdef SERIAL_DEBUG
  Serial.print("humidity = ");
  Serial.println(humidityValue);
#endif
*/

  aModulator.setFreq(aModulatorBaseFreq + 0.4313f*kModFreq1.next() + kModFreq2.next());

   kModFreq1.setFreq(.85f + (float)(humidityValue+random(50))/300.f);
}


int updateAudio(){
   aModulatorBaseFreq = (float)humidityValue/4.0f;

   //-- a short delay does a really crazy schmear of the sounds — the higher the delay, the more it sounds
   //-- like a unified sound as opposed to a series of distinct beats
   //-- the randomness adds a tinny sound at the end
   //-- note: this doesn't seem to affect the whiny sound that I don't like
    delayMicroseconds(humidityValue + 200 + random(400));

   aCarrier.setFreq((float)humidityValue/6.0f);

  int asig = aCarrier.phMod((int)aModulator.next()*(260u+aModWidth.next()));
  return (asig*(byte)aEnvelop.next())>>8;
}


void loop() {
  audioHook();
}


