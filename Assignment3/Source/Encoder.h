#pragma once
#include <JuceHeader.h>
class Encoder {
 public:
  Encoder(float sr = 48000.f, float f = 4.f) {
    phase = 0.f;
    setSampleRate(sr);
    setFreq(f);
    cnt = 0;
    text = "";
  }

  void setNextHar(float (&encoharAmp)[8], int& cntHar) {
    phase += phaseDelta;
    if (phase > 1.0f) {
      phase -= 1.f;
      cnt++;
      if (cnt >= text.length()) cnt = 0;
    }
    char ch = text[cnt];
    for (int i = 0; i < cntHar; i++) {
      if (ch == ' ') {
        encoharAmp[i] = 0.f;
      } else {
        int t = ch - 'a' + 1;
        if ((t & (1 << i)) != 0)
          encoharAmp[i] = 0.5f;
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
    cnt = 0;
  }

 private:
  float phase, phaseDelta, freq, sampleRate;
  juce::String text;
  int cnt;
};