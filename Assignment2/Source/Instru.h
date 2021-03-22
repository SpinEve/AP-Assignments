/*
  ==============================================================================

    Instru.h
    Created: 1 Mar 2021 7:22:31pm
    Author:  Mallow

  ==============================================================================
*/

#pragma once
#include "Oscillator.h"
class Instru {
 private:
  float baseFreq;       // Base frequency
  Oscillator* oscs[6];  // Oscillator of base and other harmonics
  float loud[6];        // Loudness of harmonics

 public:
  Instru(float sr = 48000.f, float _baseFreq = 440.f) {
    baseFreq = _baseFreq;
    for (int i = 0; i < 6; i++) {
      oscs[i] = new SinOsc(sr, baseFreq * (i + 1));
      // A simple distribution, though not sound very well :)
      loud[i] = 1.f / pow(2, i);
    }
  }
  float loop() {
    float res = 0;
    for (int i = 0; i < 6; i++) {
      res += loud[i] * oscs[i]->loop();
    }
    return res;
  }
  // Set base frequency
  void setbaseFreq(float _baseFreq) {
    baseFreq = _baseFreq;
    for (int i = 0; i < 6; i++) {
      oscs[i]->setFreq(baseFreq * (i + 1));
    }
  }
  ~Instru() {
    for (int i = 0; i < 6; i++) {
      delete oscs[i];
    }
  }
};