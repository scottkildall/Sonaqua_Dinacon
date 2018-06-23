/*
 *  Sonaqua_Heartbeat_NoDisplay
 *  by Scott Kildall
 *  www.kildall.com
 *  
 *  Sonaqua: Uses the heart rate sensor to make sounds. Needs some fixing on the BMP calculations
 *  
 *  Adapted from this Hackaday project
 *  https://hackaday.io/project/7008-fly-wars-a-hackers-solution-to-world-hunger/log/24646-three-dollar-ec-ppm-meter-arduino
 *  
 *  Run sensor through SDA / A4
 *  
 */

#include "MSTimer.h"

//-- leave uncommented to see serial debug messages
#define SERIAL_DEBUG

//-- Conditional defines for testing hardware
//#define SPEAKER_ONLY            // just play ascending tones, to make sure speaker works, tests speakers
//  #define SPEAKER_POT             // plays speaker according to potentiometer, tests pot

//-- Conditional to modulate a range on the pot
//#define POT_RANGE_TEST


//-- PINS
#define speakerPin (9)
#define waterLEDPin (10)
#define LEDPin (8)

#define SwitchPin (7)


#define PotPin (A0)
#define ECPower (A2)
#define ECPin (A1)   

#define HeartRateInputPin (A1)   

//-- NeoPixel colors
int r;
int b;
int g;

unsigned int toneValue;

//-- If raw EC is above this, we won't play the sounds
#define EC_SILENT_THRESHOLD (970)
#define DELAY_TIME (100)
#define MIN_TONE (60)   // lowest possible tone for Arduinos
#define MIN_TONE_THRESHOLD (60)   // Under this and we drop it down a bit more

MSTimer displayTimer = MSTimer();

//-- if this is set to TRUE, then we look at a pin for a digital input (from another Arduino), which acts like a switch
//-- OR this can be just a switch to activate
boolean bUseSwitch = false;


//-- heartbeat stuff
int Threshold = 700;            // Determine which Signal to "count as a beat", and which to ingore. 
int Signal;
unsigned long bmp;  // beats per min


MSTimer aboveThresholdTimer;
unsigned long beatsStartTime;
unsigned long numBeats;
boolean beatOn = false;
boolean bAboveThreshold = false;

void setup() {
  
   //-- pin inputs / outputs
  pinMode(ECPin,INPUT);
  pinMode(ECPower,OUTPUT);                // set pin for sourcing current
  pinMode(speakerPin, OUTPUT); 
  pinMode(LEDPin,OUTPUT);                // set pin for sourcing current
  pinMode(waterLEDPin,OUTPUT);
  
  pinMode(SwitchPin, INPUT);


  // Flash LED
  for(int i = 0; i < 6; i++ ) {
    digitalWrite(LEDPin,HIGH);
    delay(100);
    digitalWrite(LEDPin,LOW);
    delay(100);
  }
  digitalWrite(LEDPin,HIGH);

  
#ifdef SERIAL_DEBUG
  //-- no serial support for the Digispark
  Serial.begin(115200);
  Serial.println("startup");
#endif

 
  //-- speaker ground is always low
  //digitalWrite(speakerGndPin,LOW);

  // every 1000ms we will update the display, for readability
  displayTimer.setTimer(1000);
  displayTimer.start();
   
  
  digitalWrite(waterLEDPin, LOW);

  aboveThresholdTimer.setTimer(2000);
}

//-- rawEC == 0 -> max conductivity; rawEC == 1023, no conductivity
void loop() { 
  
  Signal = analogRead(HeartRateInputPin);  // Read the PulseSensor's value. 
  
  if(Signal > Threshold)   {  
    if( bAboveThreshold == false ) {
      aboveThresholdTimer.start();
      bAboveThreshold = true;
    }
    else if( aboveThresholdTimer.isExpired() ) {
      Serial.println("---");
      noTone(speakerPin);
      numBeats = 0;
    }
    
    if( beatOn == false)  {             
      tone(speakerPin,bmp);
      Serial.println("BEAT");
      
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
       Serial.println("NO BEAT");
    }   
    beatOn = false; 
   }

  delay(2);
  return;
  
  boolean bSwitchOn = digitalRead(SwitchPin);
  if( bUseSwitch == false )
    bSwitchOn = true;
    
  // Get inputs: EC and Pot Value
  unsigned int rawEC = getEC(); 
  int potValue = analogRead(PotPin);;

  if( displayTimer.isExpired() ) {
    //-- quick test
    displayTimer.start();
  }



#ifdef SPEAKER_ONLY
  speakerOnlyTest();
  return;
#endif

#ifdef SPEAKER_POT
  speakerPotTest(potValue);
  return;
#endif

#ifdef POT_RANGE_TEST
  potRangeTest(potValue);
  return;
#endif

  // Only activate if we below the EC Threshold
  if( rawEC > EC_SILENT_THRESHOLD ) { 
    noTone(speakerPin);
    digitalWrite(waterLEDPin, LOW);
    
    #ifdef SERIAL_DEBUG       
      Serial.println("-----");
    #endif
    
    return;
  }

  //-- clean this up
  boolean bPlaySound = true;
  if( bUseSwitch == true )
    bPlaySound = digitalRead(SwitchPin);
    
  
#ifdef SERIAL_DEBUG       
    Serial.print("rawEC = ");
    Serial.println(rawEC);
#endif

#ifdef SERIAL_DEBUG       
    Serial.print("Pot value = ");
    Serial.println(potValue);
#endif

  toneValue = getToneValueFromEC(rawEC);
 
  if (toneValue < 0 )
    toneValue = MIN_TONE;

    // Handle less-than-zero, will overflow to large 8-bit (65535) number
    if( toneValue > 50000 )
      toneValue = MIN_TONE;

//-- specific modulations for Currents 2018
  // this corrects for the random sampling noise that we get in normal cases
  if( toneValue == MIN_TONE ) 
    toneValue += random(2);
  else
      toneValue += random(6);

  // polluted water glitch
  
  if( rawEC < 500 ) {
    if( random(rawEC-20) < 5 )
      toneValue += random(60);
  }
  
//--

#ifdef SERIAL_DEBUG       
     Serial.print("Tone value = ");
     Serial.println(toneValue);
#endif 

    //-- emit some sort of tone based on EC
    if( bSwitchOn ) {
      tone(speakerPin, toneValue );
      digitalWrite(waterLEDPin, HIGH);
    }
    else {
      noTone(speakerPin);
      digitalWrite(waterLEDPin, LOW);
    }
     
    delay(DELAY_TIME);    
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

// expand the range of the tone value by doubling the rawEC and doing some various math to it
// this works for a sampling range where minumim EC < 400
unsigned int getToneValueFromEC(unsigned int rawEC) {
  long toneValue = ((long)rawEC - 150);

  if( toneValue  < MIN_TONE_THRESHOLD ) {
    toneValue = MIN_TONE;
    
  }
  else {
    toneValue = toneValue/2;
    
   if( toneValue  < MIN_TONE_THRESHOLD )
      toneValue = MIN_TONE;
  }

  return (unsigned int)toneValue;
}


void speakerOnlyTest() {
  for( int i = 100; i < 600; i += 10 ) {
    tone( speakerPin, i );
    delay(100);
  }
  
  return;
}

void speakerPotTest(int potValue) {
#ifdef SERIAL_DEBUG  
  Serial.println(potValue);
#endif

  tone( speakerPin, 100 + (potValue/4) );
  delay(100);
}




