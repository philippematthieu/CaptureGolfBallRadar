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
//#include <control_sgtl5000.h>
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
#define NUM_COEFFS2 100
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
  - 53,  - 62,  - 14,    20,    13,  - 1,    6,    20
};

short band_pass1[NUM_COEFFS2] = {
  279,
  385,
  472,
  410,
  182,
  -152,
  -476,
  -671,
  -673,
  -504,
  -256,
  -49,
  35,
  -17,
  -140,
  -231,
  -205,
  -44,
  187,
  374,
  423,
  324,
  165,
  86,
  182,
  424,
  661,
  713,
  505,
  141,
  -149,
  -167,
  90,
  392,
  420,
  4,
  -707,
  -1305,
  -1396,
  -920,
  -268,
  -73,
  -762,
  -2156,
  -3434,
  -3546,
  -1861,
  1328,
  4829,
  7109,
  7109,
  4829,
  1328,
  -1861,
  -3546,
  -3434,
  -2156,
  -762,
  -73,
  -268,
  -920,
  -1396,
  -1305,
  -707,
  4,
  420,
  392,
  90,
  -167,
  -149,
  141,
  505,
  713,
  661,
  424,
  182,
  86,
  165,
  324,
  423,
  374,
  187,
  -44,
  -205,
  -231,
  -140,
  -17,
  35,
  -49,
  -256,
  -504,
  -673,
  -671,
  -476,
  -152,
  182,
  410,
  472,
  385,
  279
};

short band_pass2[NUM_COEFFS2] = {
  4,  14,  24,  29,  24,  8,  -14,  -38,  -51,  -48,  -31,  -9,  0,  -15,  -59,  -116,  -159,  -158,  -100,  3,
  115,  187,  186,  117,  22,  -27,  23,  190,  423,  619,  674,  526,  205,  -170,  -443,  -488,  -289,  31,  249,
  133,  -434,  -1371,  -2384,  -3054,  -2993,  -2011,  -225,  1944,  3895,  5048,  5048,  3895,  1944,  -225,  !
  -2011,  -2993,  -3054,  -2384,  -1371,  -434,  133,  249,  31,  -289,  -488,  -443,  -170,  205,  526,  674,  619,
  423,  190,  23,  -27,  22,  117,  186,  187,  115,  3,  -100,  -158,  -159,  -116,  -59,  -15,  0,  -9,  -31
  - 48,  -51,  -38,  -14,  8,  24,  29,  24,  14,  4
};
// GUItool: end automatically generated code
AudioInputI2S           i2s2;           //xy=105,63
AudioOutputUSB           usb1;           //xy=409,382
AudioFilterFIR           firL;           //xy=276,382
AudioFilterFIR           firR;           //xy=276,382
AudioControlSGTL5000    sgtl5000_1;     //xy=265,212
//AudioConnection          patchCord1(i2s2, 0, firL, 0);
//AudioConnection          patchCord2(i2s2, 1, firR, 0);
//AudioConnection          patchCord3(firL, 0, usb1, 0);
//AudioConnection          patchCord4(firR, 0, usb1, 1);

AudioConnection          patchCord1(i2s2, 0, usb1, 0);
AudioConnection          patchCord2(i2s2, 1, usb1, 1);
// which input on the audio shield will be used?
const int myInput = AUDIO_INPUT_LINEIN;

void setup() {
  Serial.begin(38400);
  delay(1000);
  Serial.println("Teensy Audio AudioMemory()");
  AudioMemory(60);

  // Enable the audio shield, select input, and enable output
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.lineInLevel(8, 8);
  sgtl5000_1.volume(0.0);
  //firL.begin(band_pass2, NUM_COEFFS);
  //firR.begin(band_pass2, NUM_COEFFS);
  firL.begin(band_pass1, NUM_COEFFS2);
  firR.begin(band_pass1, NUM_COEFFS2);
  //sgtl5000_1.adcHighPassFilterDisable();
}

void loop() {
}
