// Color Player
// by Scott Kildall
// used for documentation
//-- supports only 2 speaker outputs with the tone library
//-- use SONAQUA boards: NeoPixel pin, speaker pin or digital input pin

//#include "Tone.h"


//-- MOZZY
#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

const char INPUT_PIN = 0; // set the input for the knob to analog pin 0

// to convey the volume level from updateControl() to updateAudio()
byte volume;
//--MOZZY


uint16_t clear, red, green, blue;



const int startSpeakerPin = 10;
const int numSpeakers = 2;

#define speakerPin (6)
#define switchPin (8)
#define waterLEDPin (10)
#define neoPixelPin (9)

#include "Adafruit_TCS34725_mod.h"
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

#define LEDPin (9)


void setup(void)
{
  Serial.begin(115200);
  Serial.println("Color View Test!");

  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }
  
  //for( int i = 0; i < numSpeakers; i++ ) {
  /*  pinMode(speakerPin, OUTPUT);
   tonePlayer[0].begin(neoPixelPin);

   pinMode(switchPin, OUTPUT);
   tonePlayer[1].begin(switchPin);*/
   


// }

  randomSeed(A0);


 pinMode(LEDPin,OUTPUT);    
  // Flash LED
  for(int i = 0; i < 6; i++ ) {
    digitalWrite(LEDPin,HIGH);
    delay(100);
    digitalWrite(LEDPin,LOW);
    delay(100);
  }
}

void loop(void)
{   
  //-- random player
  
  /*for( int i = 0; i < numSpeakers; i++ )
    tonePlayer[i].play( notes[random(24)]);

 delay(200);
 */
 
  
  checkColor();  
  


 
}

void checkColor() {
  

  tcs.setInterrupt(false);      // turn on LED

  delay(60);  // takes 50ms to read 
  
  tcs.getRawData(&red, &green, &blue, &clear);

  tcs.setInterrupt(true);  // turn off LED
  
  Serial.print("C:\t"); Serial.print(clear);
  Serial.print("\tR:\t"); Serial.print(red);
  Serial.print("\tG:\t"); Serial.print(green);
  Serial.print("\tB:\t"); Serial.print(blue);

  // Figure out some basic hex code for visualization
  uint32_t sum = clear;
  float r, g, b;
  r = red; r /= sum;
  g = green; g /= sum;
  b = blue; b /= sum;
  r *= 256; g *= 256; b *= 256;
  Serial.print("\t");
  Serial.print((int)r, HEX); Serial.print((int)g, HEX); Serial.print((int)b, HEX);
  Serial.println();

  //Serial.print((int)r ); Serial.print(" "); Serial.print((int)g);Serial.print(" ");  Serial.println((int)b );

}





