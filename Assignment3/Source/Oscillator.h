/*
  ==============================================================================

    Oscillator.h
    Created: 26 Feb 2021 3:51:42pm
    Author:  Mallow

  ==============================================================================
*/

#pragma once
#include <cmath>
constexpr auto PI = 3.1415926f;
class Oscillator {
 public:
  Oscillator(float _sr, float _freq) {
    phase = 0.0f;
    setSampleRate(_sr);
    setFreq(_freq);
  }

  float getNextSample() {
    phase += phaseDelta;
    if (phase > 1.0f) phase -= 1.0f;
    return output(phase);
  }

  // In case that skip certain numbers of loops
  float getNextSample(int num) {
    phase += phaseDelta * num;
    phase -= (int)phase;
    return output(phase);
  }

  // Mainly for phase modulation (PM) that extra shift is added to phase
  float loopwithShift(float shift) {
    phase += phaseDelta;
    if (phase > 1.0f) phase -= 1.0f;
    float tmpPhase = phase + shift;
    tmpPhase -= (int)tmpPhase;
    return output(tmpPhase);
  }

  virtual float output(float p) { return p; }
  void setSampleRate(float _sr) { sampleRate = _sr; }
  void setFreq(float _freq) {
    freq = _freq;
    phaseDelta = freq / sampleRate;
  }

 private:
  float phase, phaseDelta, freq, sampleRate;
};

// Triangle Oscillator
class TriOsc : public Oscillator {
 public:
  TriOsc(float _sr = 48000.0f, float _freq = 440.0f) : Oscillator(_sr, _freq) {}
  float output(float p) override { return 2 * fabsf(p - 0.5f); }
};

// Sine Oscillator
class SinOsc : public Oscillator {
 public:
  SinOsc(float _sr = 48000.0f, float _freq = 440.0f) : Oscillator(_sr, _freq) {}
  float output(float p) override { return sinf(2 * p * PI); }
};

// Cosine Oscillator
class CosOsc : public Oscillator {
 public:
  CosOsc(float _sr = 48000.0f, float _freq = 440.0f) : Oscillator(_sr, _freq) {}
  float output(float p) override { return cosf(2 * p * PI); }
};

// Square Oscillator
class SqrOsc : public Oscillator {
 public:
  SqrOsc(float _sr = 48000.0f, float _freq = 440.0f) : Oscillator(_sr, _freq) {}
  float output(float p) override {
    if (p < 0.5) return 1.0f;
    return -1.0f;
  }
};
