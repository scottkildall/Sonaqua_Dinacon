# Sonaqua_Phasemod_Humidity
## By Scott Kildall
### www.kildall.com
### Part of the Sonaqua project 


#### Description
With a humidty sensor, I am modulating audio synth based on the Mozzy Phasemod examples. I altered the code so that modulates the synth effects such that it sounds slower on a more humid day and faster on a less humid day.

A bit of randomness is built into the effects.

Right now, I am getting a low-level hiss, due to electronics of some sort.

For cleanup, load the WAV file into Audition and do the following:
- De-esser, multiband, threshhold = -80 (defaults)
- Adaptive Nouse Reduction, Reduce Noise By 40.00db, Noisiness = 60%, check high-quality

This seems to help, though it still is a but hissy.

#### Technical
Uses the Sparkfun Humidity Sensor Board, HIH-4030
https://www.sparkfun.com/products/9569

#### Data Collected + Sample Sounds
(1) June 24th, 2018 — recording with a humidty reading of ~675 (0-1023)
	sound file = 24_June_2018_Sonaqua_Humidity.mp3

(2) June 25th, 2018 — recording with a humidty reading of ~703 (0-1023)
	sound file = 25_June_2018_Sonaqua_Humidity.mp3
	

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


