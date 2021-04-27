#include "Synth.h"

#include <JuceHeader.h>

SynthVoice::SynthVoice() {
  sampleOsc = new SinOsc();
  sampleOsc->setSampleRate(getSampleRate());
  // DBG(getSampleRate());
}
SynthVoice::~SynthVoice() { delete sampleOsc; }
void SynthVoice::startNote(int midiNoteNumber, float velocity,
                           juce::SynthesiserSound* sound,
                           int currentPitchWheelPosition) {
  float freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
  DBG(midiNoteNumber);
  sampleOsc->setFreq(freq);
  playing = true;
}
void SynthVoice::stopNote(float velocity, bool allowTailOff) {
  clearCurrentNote();
  playing = false;
}
void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                                 int startSample, int numSamples) {
  if (playing) {
    for (int sampleIndex = startSample;
         sampleIndex < (startSample + numSamples); sampleIndex++) {
      // Oscillator
      float currentSample = sampleOsc->loop();
      // Noise Generator
      // float currentSample = random.nextFloat() * 2 - 1.0;
      for (auto i = 0; i < outputBuffer.getNumChannels(); i++) {
        outputBuffer.addSample(i, sampleIndex,
                               currentSample * 0.5 * (volume / 100.f));
      }
    }
  }
}
bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound) {
  return dynamic_cast<SynthSound*>(sound) != nullptr;
}
void SynthVoice::setVolume(float _volume) { volume = _volume; }
