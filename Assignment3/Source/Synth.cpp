#include "Synth.h"

#include <JuceHeader.h>

SynthVoice::SynthVoice() {
  auto sr = getSampleRate();
  sampleOsc = new SinOsc();
  sampleOsc->setSampleRate(sr);
  env.setSampleRate(sr);
  envPara.attack = 0.1f;
  envPara.decay = 0.1f;
  envPara.sustain = 1.0f;
  envPara.release = 0.5f;
  env.setParameters(envPara);
}
SynthVoice::~SynthVoice() { delete sampleOsc; }
void SynthVoice::startNote(int midiNoteNumber, float velocity,
                           juce::SynthesiserSound* sound,
                           int currentPitchWheelPosition) {
  float freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
  sampleOsc->setFreq(freq);
  env.noteOn();
  playing = true;
  isOff = false;
}
void SynthVoice::stopNote(float velocity, bool allowTailOff) {
  env.noteOff();
  isOff = true;
}
void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                                 int startSample, int numSamples) {
  if (playing) {
    for (auto i = startSample; i < (startSample + numSamples); i++) {
      float currentSample = sampleOsc->getNextSample() * env.getNextSample();
      for (auto ch = 0; ch < outputBuffer.getNumChannels(); ch++) {
        outputBuffer.addSample(ch, i, currentSample * 0.5 * (volume / 100.f));
      }
      if (isOff && !env.isActive()) {
        playing = false;
        clearCurrentNote();
        break;
      }
    }
  }
}
bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound) {
  return dynamic_cast<SynthSound*>(sound) != nullptr;
}
void SynthVoice::setVolume(float _volume) { volume = _volume; }
