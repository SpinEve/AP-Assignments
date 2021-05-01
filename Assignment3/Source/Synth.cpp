#include "Synth.h"

#include <JuceHeader.h>

SynthVoice::SynthVoice(juce::AudioProcessorValueTreeState& vts)
    : valueTreeState(vts) {
  auto sr = getSampleRate();

  carrFreq = valueTreeState.getRawParameterValue("carrFreq");
  noiseLevel = valueTreeState.getRawParameterValue("noiseLevel");
  gain = valueTreeState.getRawParameterValue("gain");

  playing = false;
  isOff = false;
  harEnabled = false;
  cntHar = 8;
  moduType = 1;

  carrOsc = new SinOsc();
  carrOsc->setSampleRate(sr);

  midiOsc = new SinOsc();
  midiOsc->setSampleRate(sr);
  midiOscType = 1;

  LFO1 = new SinOsc();
  LFO1->setSampleRate(sr);
  LFO1ModuType = 1;
  LFO1Amp = 0.f;

  env.setSampleRate(sr);
  setADSR(0.1f, 0.1f, 1.0f, 0.5f);
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
  midiOsc->setDefFreq(freq);
  if (harEnabled) {
    for (int i = 0; i < cntHar; i++) {
      harOsc[i]->setDefFreq(freq * (i + 2));
      harAmp[i] = 1.f / (i + 1);
    }
  }
  env.noteOn();
  playing = true;
  isOff = false;
}
void SynthVoice::stopNote(float velocity, bool allowTailOff) {
  env.noteOff();
  isOff = true;
}
float modulate(Oscillator* carrOsc, float sample, int type) {
  if (type <= 1) {  // No modulation
    sample = carrOsc->getNextSample();
  } else if (type == 2) {  // FM
    carrOsc->setFreq(carrOsc->getDefFreq() * (1 + sample));
    sample = carrOsc->getNextSample();
  } else if (type == 3) {  // PM
    sample = carrOsc->getShiftedSample(sample);
    carrOsc->getNextSample();
  } else if (type == 4) {  // AM
    sample = carrOsc->getNextSample() * (1 + sample);
  }
  return sample;
}
void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                                 int startSample, int numSamples) {
  if (playing) {
    for (auto i = startSample; i < (startSample + numSamples); i++) {
      float LFOSample = LFO1->getNextSample() * LFO1Amp;
      currentSample = modulate(midiOsc, LFOSample, LFO1ModuType);
      if (harEnabled) {
        float harSample = 0.f;
        for (auto j = 0; j < cntHar; j++)
          harSample += modulate(harOsc[j], LFOSample, LFO1ModuType) * harAmp[j];
        currentSample += harSample;
      }
      // Modulation Part
      if (moduType > 1)
        currentSample = modulate(carrOsc, currentSample, moduType);
      // Add some noise
      currentSample = (2 * random.nextFloat() - 1) * (*noiseLevel) +
                      currentSample * (1.f - (*noiseLevel));
      // Envelope
      currentSample *= env.getNextSample();
      // Gain, halved so not too loud
      currentSample *= (*gain) / 2;
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
void SynthVoice::updateState() { carrOsc->setDefFreq(*carrFreq); }
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
void SynthVoice::setLFO1(int type, int mt, float freq, float amp) {
  delete LFO1;
  LFO1 = selectOsc(type);
  LFO1->setSampleRate(getSampleRate());
  LFO1->setDefFreq(freq);
  LFO1ModuType = mt;
  LFO1Amp = amp;
}
