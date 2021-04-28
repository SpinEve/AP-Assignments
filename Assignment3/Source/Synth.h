#pragma once
#include <JuceHeader.h>

#include "Oscillator.h"

class SynthSound : public juce::SynthesiserSound {
 public:
  bool appliesToNote(int) override { return true; }
  bool appliesToChannel(int) override { return true; }
};

class SynthVoice : public juce::SynthesiserVoice {
 public:
  SynthVoice();
  void startNote(int midiNoteNumber, float velocity,
                 juce::SynthesiserSound* sound,
                 int currentPitchWheelPosition) override;
  void stopNote(float velocity, bool allowTailOff) override;
  void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample,
                       int numSamples) override;
  void pitchWheelMoved(int) override {}
  void controllerMoved(int, int) override {}
  bool canPlaySound(juce::SynthesiserSound* sound) override;
  void setCarrFreq(float _carrFreq);
  void setADSR(float a, float d, float s, float r);
  ~SynthVoice();

 private:
  bool playing = false;
  bool isOff = false;
  // juce::Random random;
  juce::ADSR env;
  juce::ADSR::Parameters envPara;

  float carrFreq = 440.f;
  SinOsc *carrOsc, *midiOsc;
};
