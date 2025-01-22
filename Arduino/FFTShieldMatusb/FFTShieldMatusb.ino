// Record sound as raw data to a SD card, and play it back.
//
// Requires the audio shield:
//   http://www.pjrc.com/store/teensy3_audio.html
//
// Three pushbuttons need to be connected:
//   Record Button: pin 0 to GND
//   Stop Button:   pin 1 to GND
//   Play Button:   pin 2 to GND
//
// This example code is in the public domain.

#include <Bounce.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

//#include <SerialFlash.h>

/*
  FIR filter designed with
  http://t-filter.appspot.com

  sampling frequency: 44100 Hz

  fixed point precision: 16 bits

  1300 Hz - 5000 Hz
  gain = 1
  desired ripple = 5 dB
  actual ripple = n/a

*/

#define NUM_COEFFS 64
//nt=64;
//f1=2000;
//f2=5000;
//fsh=44100/2;
//forder = 64;
//[wft,wfm,fr] = wfir('bp',forder,[f1,f2]/fsh,'hm',[-1 -1]);
//ihh=round(wft*2^15-1)
short band_pass[NUM_COEFFS] = {
    20,    6,  - 1,    13,    20,  - 14,  - 62,  - 53,    6,    19,  
  - 28,  - 3,    145,    215,    44,  - 163,  - 121,    11,  - 152,  - 461, 
  - 296,    372,    704,    285,  - 29,    506,    877,  - 462,  - 2715, 
  - 2874,    308,    3883,    3883,    308,  - 2874,  - 2715,  - 462,  
    877,    506,  - 29,    285,    704,    372,  - 296,  - 461,  - 152,  
    11,  - 121,  - 163,    44,    215,    145,  - 3,  - 28,    19,    6, 
  - 53,  - 62,  - 14,    20,    13,  - 1,    6,    20 };
// GUItool: end automatically generated code

AudioInputI2S           i2s2;           //xy=105,63
AudioRecordQueue        queue1;         //xy=281,63
AudioPlaySdRaw          playRaw1;       //xy=302,157

AudioFilterFIR           fir1;           //xy=276,382
AudioFilterFIR           fir2;           //xy=276,382
AudioFilterFIR           fir3;           //xy=276,382

AudioConnection         patchCord1(i2s2, 1, queue1, 0);

AudioControlSGTL5000    sgtl5000_1;     //xy=265,212

AudioAnalyzeFFT256      fft256_1;      //xy=439,211
//AudioConnection         patchCord2(i2s2, 1, fir1, 0);
//AudioConnection         patchCord3(fir1, fft256_1);
AudioConnection         patchCord3(i2s2, 1, fft256_1, 0);
AudioAnalyzeFFT1024     fft1024_1;      //xy=439,211
//AudioConnection         patchCord3(i2s2, 1, fft1024_1, 0);

AudioOutputUSB           usb1;           //xy=409,382
AudioConnection          patchCord4(i2s2, 0, usb1, 0);
AudioConnection          patchCord5(i2s2, 1, usb1, 1);

// GUItool: end automatically generated code

// For a stereo recording version, see this forum thread:
// https://forum.pjrc.com/threads/46150?p=158388&viewfull=1#post158388

// Bounce objects to easily and reliably read the buttons
Bounce buttonRecord = Bounce(0, 8);
Bounce buttonStop =   Bounce(1, 8);  // 8 = 8 ms debounce time
Bounce buttonPlay =   Bounce(2, 8);

// which input on the audio shield will be used?
const int myInput = AUDIO_INPUT_LINEIN;

// Remember which mode we're doing
int mode = 1;  // 0=stopped, 1=recording, 2=playing
int tmp = 0;
bool swingingOK = false, ResetedButton = true;
float fftAccumulator, valFFT, ampMax;
float theMax, theMax2, theMax3, theMax4, theMax5, theMax6, theMax7, theMax8, theMax9;
const float LevelDetection = 00.0;
void setup() {
  // Configure the pushbutton pins
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);

  Serial.begin(38400);
  delay(1000);
  Serial.println("Teensy Audio AudioMemory()");
  AudioMemory(60);
  fir1.begin(band_pass, NUM_COEFFS);

  // Enable the audio shield, select input, and enable output
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(1);

  theMax = 0.0;
  valFFT = 0.0;
  ampMax = 0.0;
  swingingOK = false;
  ResetedButton = true;
  mode = 1;
  //startRecording();
}


void loop() {
  // If we're playing or recording, carry on...
  if (mode == 1) {
    //continueRecording();//StartReccording
    //continueFFT();
  }
  Serial.println("startRecording");
}

void continueFFT() {
  float n;
  int i;
  if (fft256_1.available() && !swingingOK) { // recherche du début de swing = backswing
    theMax = 0.0;
    // each time new FFT data is available
    // print it all to the Arduino Serial Monitor
    for (i = 10; i < 100; i++) {
      fftAccumulator = fft256_1.read(i) * 1000; // calcul du niveau zmplitude ==> Grace au filtre
      if (fftAccumulator >= LevelDetection) { // limite l'amplitude min pour la recherche de freq
        swingingOK = true;
        valFFT = float(44100 * (i - 1 )) / (256.0); // calcul de la fréquence
        theMax = max(valFFT, theMax)/19.49; // freq Max
        Serial.print(fftAccumulator*3);
        //Serial.println();
        Serial.print(" - ");
        Serial.print(valFFT/19.49);
        Serial.println();
        //ampMax = max(fftAccumulator, ampMax); // Amplitude Max
      }
    }
    Serial.print(theMax);
    Serial.print(" - ");
    Serial.print(ampMax);
    Serial.println();
  }
  if (swingingOK)
  {
    ResetedButton = false;
    // calcul de la vitesse du club et de la balle
    //
    //
    //fin du swing
    swingingOK = false;
  }
  if (ResetedButton)
  {
    ResetedButton = false;
    swingingOK = false;
  }
}

