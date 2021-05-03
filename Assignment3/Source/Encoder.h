/*
  ==============================================================================

    Encoder.h
    Created: 1 May 2021 3:51:42pm
    Author:  Mallow

  ==============================================================================
*/
#pragma once
#include <JuceHeader.h>

/// Encoding string to harmonics amps in bit
class Encoder {
 public:
  Encoder(float sr = 48000.f, float f = 4.f) {
    phase = 0.f;
    setSampleRate(sr);
    setFreq(f);
    cnt = 0;
    text = "";
  }
  /** Modify encoHarAmp in a loop.
   */
  void setNextHar(float (&encoharAmp)[8], int& cntHar) {
    phase += phaseDelta;
    if (phase > 1.0f) {
      phase -= 1.f;
      cnt++;                              // Next character
      if (cnt >= text.length()) cnt = 0;  // Back to first char
    }
    char ch = text[cnt];
    for (int i = 0; i < cntHar; i++) {
      if (ch == ' ') {
        encoharAmp[i] = 0.f;  // No amps
      } else {
        int t = ch - 'a' + 1;
        if ((t & (1 << i)) != 0)  // if '1' in 2-bit represent
          encoharAmp[i] = 0.5f;   // Give it amps
        else
          encoharAmp[i] = 0.f;
      }
    }
  }

  void setSampleRate(float sr) { sampleRate = sr; }
  void setFreq(float f) {
    freq = f;
    phaseDelta = freq / sampleRate;
  }

  void setText(juce::String t) {
    text = t;
    cnt = 0;  // Reset cnt
  }

 private:
  float phase, phaseDelta, freq, sampleRate;
  juce::String text;
  int cnt;
};