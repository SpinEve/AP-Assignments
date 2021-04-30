#include "Synth.h"

#include <JuceHeader.h>

SynthVoice::SynthVoice() {
  gain = 0.5f;
  carrFreq = 440.f;
  noiseLevel = 0.f;
  playing = false;
  isOff = false;
  harEnabled = false;
  cntHar = 8;

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
  // delete[] harOsc;
}
void SynthVoice::startNote(int midiNoteNumber, float velocity,
                           juce::SynthesiserSound* sound,
                           int currentPitchWheelPosition) {
  auto freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
  midiOsc->setFreq(freq);
  if (harEnabled) {
    for (int i = 0; i < cntHar; i++) harOsc[i]->setFreq(freq * (i + 2));
  }
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
      currentSample = midiOsc->getNextSample();
      // Harmonics Part
      if (harEnabled) {
        float harSample = 0.f;
        for (int i = 0; i < cntHar; i++)
          harSample += harOsc[i]->getNextSample();
        currentSample += 0.5f * harSample;
      }
      // Modulation Part
      if (moduType == 1) {  // No Modulation
        // Do nothing
      } else if (moduType == 2) {  // FM
        carrOsc->setFreq(carrOsc->getDefFreq() * (1 + currentSample));
        currentSample = carrOsc->getNextSample();
      } else if (moduType == 3) {  // PM
        currentSample = carrOsc->getShiftedSample(currentSample);
      } else if (moduType == 4) {  // AM
        currentSample = carrOsc->getNextSample() * (1 + currentSample);
      }
      // Add some noise
      currentSample =
          random.nextFloat() * noiseLevel + currentSample * (1.f - noiseLevel);
      // Envelope
      currentSample *= env.getNextSample();
      // Gain, halved so not too loud
      currentSample *= gain / 2;
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
Oscillator* selectOsc(int ot) {
  if (ot == 1)
    return new SinOsc();
  else if (ot == 2)
    return new CosOsc();
  else if (ot == 3)
    return new TriOsc();
  else if (ot == 4)
    return new SqrOsc();
  else if (ot == 5)
    return new SawOsc();
  else
    return new NoiseOsc();
}
void SynthVoice::setMidiOscType(int ot) {
  // No change
  if (midiOscType == ot) return;
  midiOscType = ot;

  delete midiOsc;
  midiOsc = selectOsc(midiOscType);
  midiOsc->setSampleRate(getSampleRate());
  // also change harmonics if it's enabled
  if (harEnabled) {
    for (int i = 0; i < cntHar; i++) {
      delete harOsc[i];
      harOsc[i] = selectOsc(midiOscType);
      harOsc[i]->setSampleRate(getSampleRate());
    }
  }
}
void SynthVoice::setCarrOscType(int ot) {
  if (carrOscType == ot) return;
  carrOscType = ot;

  delete carrOsc;
  carrOsc = selectOsc(carrOscType);
  carrOsc->setSampleRate(getSampleRate());
}
void SynthVoice::setNoiseLevel(float nl) { noiseLevel = nl; }
void SynthVoice::setGain(float g) { gain = g; };
void SynthVoice::setHar(bool enabled) {
  // No change
  if (enabled == harEnabled) return;
  if (enabled) {  // Enable it
    harEnabled = true;
    for (int i = 0; i < cntHar; i++) {
      delete harOsc[i];
      harOsc[i] = selectOsc(midiOscType);
      harOsc[i]->setSampleRate(getSampleRate());
    }
  } else {  // Disable it
    harEnabled = false;
    for (int i = 0; i < cntHar; i++) delete harOsc[i];
  }
}
