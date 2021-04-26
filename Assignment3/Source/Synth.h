#pragma once
#include <JuceHeader.h>

class SynthSound : public juce::SynthesiserSound {
 public:
  bool appliesToNote(int) override { return true; }
  bool appliesToChannel(int) override { return true; }
};

class SynthVoice : public juce::SynthesiserVoice {
 public:
  SynthVoice() {}
  void startNote(int midiNoteNumber, float velocity,
                 juce::SynthesiserSound* sound,
                 int currentPitchWheelPosition) override {
    float freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    playing = true;
  }
  void stopNote(float velocity, bool allowTailOff) override {
    clearCurrentNote();
    playing = false;
  }
  void renderNextBlock(juce::AudioBuffer<double>& outputBuffer, int startSample,
                       int numSamples) override {
    if (playing) {
      for (int sampleIndex = startSample;
           sampleIndex < (startSample + numSamples); sampleIndex++) {
        // TODO: Oscillator
        // Noise Generator
        float currentSample = random.nextFloat() * 2 - 1.0;
        for (int chan = 0; chan < outputBuffer.getNumChannels(); chan++) {
          outputBuffer.addSample(chan, sampleIndex,
                                 currentSample * 0.2 * (volume / 100.f));
        }
      }
    }
  }
  void pitchWheelMoved(int) override {}
  void controllerMoved(int, int) override {}
  bool canPlaySound(juce::SynthesiserSound* sound) override {
    return dynamic_cast<SynthSound*>(sound) != nullptr;
  }
  void setVolume(float _volume) { volume = _volume; }

 private:
  bool playing = false;
  float volume = 50.f, freq = 440.f;
  juce::Random random;
};
