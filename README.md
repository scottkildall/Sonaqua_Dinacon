# Sonaqua_Dinacon
## By Scott Kildall
### www.kildall.com
### Part of the Sonaqua project 


#### Description

Sonaqua asks the question: What does water quality sound like?

With this ongoing installation and workshop series, I collect river water samples from across the globe with the assistance of volunteers and workshop participants. Each water sample contains different minerals and will conduct an electrical current (EC) at different rates. Measurements of EC correlate to total dissolved solids (TDS), which is often an indicator of heavy metals pollution by industrial and mining process. So, the more ecologically impacted a river system, the more electrical charge it will likely conduct.

Using a simple Arduino circuit, this artwork creates sonification orchestral arrangements of water samples. As an autonomous installation, it acts as an electronic organism, emitting patterns of noise and reflects the interconnectedness of our ecological system. As an installation, it can also include direct interactivity through buttons and sensors to engage in a dialogue of scientific-based curiosity. The installation can morph into different forms, depending on context.

I am also teaching this as a series of workshops where participants solder circuits and construct enclosures to build handheld Sonaqua devices, so that we can create large-scale performances.



#### Files + Examples
Note: Incomplete directory are from pre-Dinacon tests

#####Working:
######SonaquaModulePlayer_v06:
The basic example. This one does not use Mozzi and also will output to the LED backpack. This is a great first tester.

######Sonaqua_Synth_Simulator:
A series of synth effects for simulation, before plugging into electronic sensors

######Sonaqua_Phasemod_Humidity
Humidity Sensor with Phasmod.

#####In Progress:
######Sonaqua_Electric_Noise:
A working example of Mozzi using analog input for current flow (usually through water). 

######Sonaqua_Heartbeat:
A working example which uses the Adafruit heart rate sensor. BMP calculation is buggy

#####Sonaqua_SoilSensor:
Uses the Sparkfun Soil Moisture Sensor to make sounds

#####Sonaqua_Electrodes:
Crude sketch with electrodes, good for reading plant leaves. EC values are in range of 700-950

######Sonaqua_Detuned_Beats_Wash:
In progress, using Sonaqua with Mozzi

######Sonaqua_Alcohol_Sensor:
In progress

######Sonaqua_Phasemod_Envelope:
In progress, using Sonaqua with Mozzi

######Sonaqua_PWM_Phasing:
In progress, using Sonaqua with Mozzi




#### Technical


#### License

Sonaqua (c) by Scott Kildall

Sonaqua is licensed under a
Creative Commons Attribution-NonCommercial 3.0 Unported License.

You should have received a copy of the license along with this
work.  If not, see <http://creativecommons.org/licenses/by-nc/3.0/>.

#### This uses the Mozzy Libraries
##### http://sensorium.github.io/Mozzi/
##### by Tim Barass
##### https://github.com/sensorium/Mozzi
##### Licensed under a Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0) license


