#pragma once
#include <JuceHeader.h>

#include "Oscillator.h"

class SynthSound : public juce::SynthesiserSound {
 public:
  bool appliesToNote(int) override { return true; }
  bool appliesToChannel(int) override { return true; }
};

class SynthVoice : public juce::SynthesiserVoice,
                   public juce::AudioProcessorValueTreeState::Listener {
 public:
  SynthVoice(juce::AudioProcessorValueTreeState&);
  void startNote(int midiNoteNumber, float velocity,
                 juce::SynthesiserSound* sound,
                 int currentPitchWheelPosition) override;
  void stopNote(float velocity, bool allowTailOff) override;
  void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample,
                       int numSamples) override;
  void pitchWheelMoved(int) override {}
  void controllerMoved(int, int) override {}
  bool canPlaySound(juce::SynthesiserSound* sound) override;
  void parameterChanged(const juce::String& parameterID,
                        float newValue) override;

  void setModuType(int mt);
  void setMidiOscType(int ot);
  void setCarrOscType(int ot);
  void setHar(bool enabled);
  void setLFO1(int type, int moduType, float freq, float amp);

  ~SynthVoice();

 private:
  bool playing, isOff, harEnabled;
  float currentSample;
  std::atomic<float>* gain = nullptr;
  std::atomic<float>* carrFreq = nullptr;
  std::atomic<float>* noiseLevel = nullptr;
  std::atomic<float>* attack = nullptr;
  std::atomic<float>* decay = nullptr;
  std::atomic<float>* sustain = nullptr;
  std::atomic<float>* release = nullptr;

  int moduType, carrOscType, midiOscType, cntHar;
  Oscillator *carrOsc, *midiOsc, *LFO1;
  Oscillator* harOsc[8];

  int LFO1ModuType;
  float LFO1Amp;
  float harAmp[8];
  juce::Random random;
  juce::ADSR env;
  juce::ADSR::Parameters envPara;
  juce::AudioProcessorValueTreeState& valueTreeState;
};
