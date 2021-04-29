#include "Synth.h"

#include <JuceHeader.h>

SynthVoice::SynthVoice() {
  auto sr = getSampleRate();
  carrOsc = new SinOsc();
  carrOsc->setSampleRate(sr);
  carrOsc->setDefFreq(carrFreq);

  midiOsc = new SinOsc();
  midiOsc->setSampleRate(sr);

  env.setSampleRate(sr);
  setADSR(0.1f, 0.1f, 1.0f, 0.2f);  // TODO: ADSR slider?
}
SynthVoice::~SynthVoice() {
  delete carrOsc;
  delete midiOsc;
}
void SynthVoice::startNote(int midiNoteNumber, float velocity,
                           juce::SynthesiserSound* sound,
                           int currentPitchWheelPosition) {
  auto freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
  midiOsc->setFreq(freq);
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
      float currentSample;
      // Modulation Part
      if (moduType == 1) {  // No Modulation
        currentSample = midiOsc->getNextSample();
      } else if (moduType == 2) {  // FM
        carrOsc->setFreq(carrOsc->getDefFreq() *
                         (1 + midiOsc->getNextSample()));
        currentSample = carrOsc->getNextSample();
      } else if (moduType == 3) {  // PM
        currentSample = carrOsc->getShiftedSample(midiOsc->getNextSample());
      } else if (moduType == 4) {  // AM
        currentSample =
            carrOsc->getNextSample() * (1 + midiOsc->getNextSample());
      }
      // Add some noise
      currentSample =
          random.nextFloat() * noiseLevel + currentSample * (1.f - noiseLevel);
      // Envelope
      currentSample *= env.getNextSample();
      // Gain
      // TODO: gain slider
      currentSample *= 0.2;
      // Render
      for (auto ch = 0; ch < outputBuffer.getNumChannels(); ch++) {
        outputBuffer.addSample(ch, i, currentSample);
      }
      // Check if this note should be cleaned
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
void SynthVoice::setCarrFreq(float cf) {
  carrFreq = cf;
  carrOsc->setDefFreq(carrFreq);
}
void SynthVoice::setADSR(float a, float d, float s, float r) {
  envPara.attack = a;
  envPara.decay = d;
  envPara.sustain = s;
  envPara.release = r;
  env.setParameters(envPara);
}
void SynthVoice::setModuType(int mt) {
  moduType = mt;
  carrOsc->clear();  // Reset the carrier osc
}
void SynthVoice::setMidiOscType(int ot) {
  if (midiOscType == ot) return;

  delete midiOsc;
  if (ot == 1)
    midiOsc = new SinOsc();
  else if (ot == 2)
    midiOsc = new CosOsc();
  else if (ot == 3)
    midiOsc = new TriOsc();
  else if (ot == 4)
    midiOsc = new SqrOsc();
  else if (ot == 5)
    midiOsc = new SawOsc();
  else if (ot == 6)
    midiOsc = new NoiseOsc();
  midiOsc->setSampleRate(getSampleRate());
}
void SynthVoice::setNoiseLevel(float nl) { noiseLevel = nl; }
