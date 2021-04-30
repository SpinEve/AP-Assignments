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
  Oscillator(float sr, float f) {
    phase = 0.f;
    setSampleRate(sr);
    setDefFreq(f);
    setFreq(f);
  }

  float getNextSample() {
    phase += phaseDelta;
    if (phase > 1.0f) phase -= 1.f;
    return output(phase);
  }

  // In case that skip certain numbers of loops
  float getNextSample(int num) {
    phase += phaseDelta * num;
    phase -= (int)phase;
    return output(phase);
  }

  // For phase modulation (PM) where extra shift is added to phase
  float getShiftedSample(float shift) {
    phase += phaseDelta;
    if (phase > 1.0f) phase -= 1.f;
    float tmpPhase = phase + shift;
    tmpPhase -= (int)tmpPhase;
    return output(tmpPhase);
  }

  virtual float output(float p) { return p; }
  void setSampleRate(float sr) { sampleRate = sr; }
  void setFreq(float f) {
    freq = f;
    phaseDelta = freq / sampleRate;
  }
  void setDefFreq(float f) {
    defFreq = f;
    setFreq(defFreq);
  }
  float getDefFreq() { return defFreq; }
  void clear() {
    setFreq(defFreq);
    phase = 0.f;
  }

 private:
  float phase, phaseDelta, freq, defFreq, sampleRate;
};

// Triangle Oscillator
class TriOsc : public Oscillator {
 public:
  TriOsc(float sr = 48000.f, float f = 440.f) : Oscillator(sr, f) {}
  float output(float p) override { return 2 * fabsf(p - 0.5f); }
};

// Sine Oscillator
class SinOsc : public Oscillator {
 public:
  SinOsc(float sr = 48000.f, float f = 440.f) : Oscillator(sr, f) {}
  float output(float p) override { return sinf(2 * p * PI); }
};

// Cosine Oscillator
class CosOsc : public Oscillator {
 public:
  CosOsc(float sr = 48000.0f, float f = 440.f) : Oscillator(sr, f) {}
  float output(float p) override { return cosf(2 * p * PI); }
};

// Square Oscillator
class SqrOsc : public Oscillator {
 public:
  SqrOsc(float sr = 48000.f, float f = 440.f) : Oscillator(sr, f) {}
  float output(float p) override {
    if (p < 0.5) return 1.f;
    return -1.f;
  }
};

// Sawtooth Oscillator
class SawOsc : public Oscillator {
 public:
  SawOsc(float sr = 48000.f, float f = 440.f) : Oscillator(sr, f) {}
  float output(float p) override { return 2 * p - 1; }
};

// Noise Oscillator
class NoiseOsc : public Oscillator {
 public:
  NoiseOsc(float sr = 48000.f, float f = 440.f) : Oscillator(sr, f) {}
  float output(float p) override {
    return 2 * ((float)rand() / RAND_MAX) - 1.f;
  }
};

// "One" Oscillator, only output 1
class OneOsc : public Oscillator {
 public:
  OneOsc(float sr = 48000.f, float f = 440.f) : Oscillator(sr, f) {}
  float output(float p) override { return 1.f; }
};
