#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

#include <arduinoFFT.h>
#include "FastLED.h"

#define SAMPLES 1024
#define SAMPLING_FREQ 500000
#define NOISE         500  

double vReal[SAMPLES];
double vImag[SAMPLES];
uint8_t step1 = 0;
uint8_t step2 = 0;
unsigned long newTime;
unsigned int sampling_period_us;

#define LENGTH 10
#define FASTLED_ESP8266_RAW_PIN_ORDER
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define PIN1 1

CRGB strip[LENGTH];
int brightness = 0;


int lowFreq = 0;
int midFreq = 0;

arduinoFFT FFT = arduinoFFT(vReal, vImag, SAMPLES, SAMPLING_FREQ);

void setup() {
  Serial.begin(115200);
  sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQ));

  
    FastLED.addLeds<LED_TYPE, PIN1, COLOR_ORDER>(strip, LENGTH).setCorrection(TypicalLEDStrip);
    FastLED.setCorrection(TypicalPixelString);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 11600);

    fill_solid(strip, LENGTH, CRGB(255, 0, 255));
    FastLED.setBrightness(brightness);

    FastLED.show();

}

void loop() {
  brightness *= 0;
  lowFreq = 0;
  midFreq = 0;
  analogRead(0);
  // put your main code here, to run repeatedly:
  for (int i = 0; i < SAMPLES; i++) {
      newTime = micros();
      vReal[i] = analogRead(0); // A conversion takes about 9.7uS on an ESP32
      vImag[i] = 0;
      //while ((micros() - newTime) < sampling_period_us) {  /*chill*/  }
    }
    // Compute FFT
    FFT.DCRemoval();
    FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(FFT_FORWARD);
    FFT.ComplexToMagnitude();

    for (int i = 2; i < SAMPLES/2; i++) {
      if (vReal[i] > NOISE) {
        //Serial.printf("%d.%d: %d Hz; Value: %f \n", step1, step2, i, vReal[i]);
        //step1++; step2++;
        //if(i<=84) lowFreq += (int)vReal[i];
        if(i>0 && i<=500) midFreq += (int)vReal[i]; //82 & 282
      }
    }

    for (int k = 0; k < midFreq; k += 1000){
      Serial.print("|");
    }
    Serial.println();

    if(midFreq > 60000){
      brightness = 255;
      
    }
    FastLED.setBrightness(brightness);
    FastLED.show();
    }
