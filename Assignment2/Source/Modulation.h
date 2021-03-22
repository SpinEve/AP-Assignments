/*
  ==============================================================================

    Modulation.h
    Created: 28 Feb 2021 5:10:00pm
    Author:  Mallow
    Intro: Modulation classes

  ==============================================================================
*/

#pragma once
#include "Oscillator.h"
class Modulation {
 private:
  Oscillator* carrier;  // Carrier signal

 public:
  Modulation(float _sr, float _carrFreq) {
    carrier = new SinOsc(_sr, _carrFreq);
  }
  Modulation(Oscillator* _carrier) { carrier = _carrier; }
  float output() { return carrier->loop(); }
  // For phase modulation with shifted output
  float output(float shift) { return carrier->loopwithShift(shift); }
  virtual float loop(float signal) { return output(); }
  ~Modulation() { delete carrier; }
};

/**
 * Amplitude modulation
 * Output = carrier(t) * (1 + signal(t))
 */
class AM : public Modulation {
 public:
  AM(float _sr = 48000.f, float _carrFreq = 440.f)
      : Modulation(_sr, _carrFreq) {}
  AM(Oscillator* _carrier) : Modulation(_carrier) {}
  float loop(float signal) override { return output() * (1 + signal); }
};

/**
 * Phase modulation
 * Output = carrier(t + signal(t))
 */
class PM : public Modulation {
 public:
  PM(float _sr = 48000.f, float _carrFreq = 440.f)
      : Modulation(_sr, _carrFreq) {}
  PM(Oscillator* _carrier) : Modulation(_carrier) {}
  float loop(float signal) override { return output(signal); }
};
