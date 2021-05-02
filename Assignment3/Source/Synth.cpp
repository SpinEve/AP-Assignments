#include "Synth.h"

#include <JuceHeader.h>

SynthVoice::SynthVoice(juce::AudioProcessorValueTreeState& vts)
    : valueTreeState(vts) {
  auto sr = getSampleRate();

  carrFreq = valueTreeState.getRawParameterValue("carrFreq");
  valueTreeState.addParameterListener("carrFreq", this);

  noiseLevel = valueTreeState.getRawParameterValue("noiseLevel");
  gain = valueTreeState.getRawParameterValue("gain");

  moduType = valueTreeState.getRawParameterValue("moduType");

  midiOscType = valueTreeState.getRawParameterValue("midiOscType");
  valueTreeState.addParameterListener("midiOscType", this);

  carrOscType = valueTreeState.getRawParameterValue("carrOscType");
  valueTreeState.addParameterListener("carrOscType", this);

  attack = valueTreeState.getRawParameterValue("attack");
  decay = valueTreeState.getRawParameterValue("decay");
  sustain = valueTreeState.getRawParameterValue("sustain");
  release = valueTreeState.getRawParameterValue("release");

  valueTreeState.addParameterListener("attack", this);
  valueTreeState.addParameterListener("decay", this);
  valueTreeState.addParameterListener("sustain", this);
  valueTreeState.addParameterListener("release", this);

  LFO1Type = valueTreeState.getRawParameterValue("LFO1Type");
  LFO1Freq = valueTreeState.getRawParameterValue("LFO1Freq");
  LFO1Amp = valueTreeState.getRawParameterValue("LFO1Amp");
  LFO1Modu = valueTreeState.getRawParameterValue("LFO1Modu");

  valueTreeState.addParameterListener("LFO1Type", this);
  valueTreeState.addParameterListener("LFO1Freq", this);

  playing = false;
  isOff = false;
  harEnabled = false;
  cntHar = 8;

  carrOsc = new SinOsc();
  carrOsc->setSampleRate(sr);

  midiOsc = new SinOsc();
  midiOsc->setSampleRate(sr);

  LFO1 = new SinOsc();
  LFO1->setSampleRate(sr);

  env.setSampleRate(sr);
}
SynthVoice::~SynthVoice() {
  delete carrOsc;
  delete midiOsc;
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
      float LFOSample = LFO1->getNextSample() * (*LFO1Amp);
      currentSample = modulate(midiOsc, LFOSample, (int)(*LFO1Modu));
      if (harEnabled) {
        float harSample = 0.f;
        for (auto j = 0; j < cntHar; j++)
          harSample +=
              modulate(harOsc[j], LFOSample, (int)(*LFO1Modu)) * harAmp[j];
        currentSample += harSample;
      }
      // Modulation Part
      if ((*moduType) > 1)
        currentSample = modulate(carrOsc, currentSample, (int)(*moduType));
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
void SynthVoice::parameterChanged(const juce::String& parameterID,
                                  float newValue) {
  if (parameterID == "carrFreq") {
    carrOsc->setDefFreq(newValue);
  } else if (parameterID == "attack") {
    envPara.attack = newValue;
    env.setParameters(envPara);
  } else if (parameterID == "decay") {
    envPara.decay = newValue;
    env.setParameters(envPara);
  } else if (parameterID == "sustain") {
    envPara.sustain = newValue;
    env.setParameters(envPara);
  } else if (parameterID == "release") {
    envPara.release = newValue;
    env.setParameters(envPara);
  } else if (parameterID == "midiOscType") {
    delete midiOsc;
    midiOsc = selectOsc((int)newValue);
    midiOsc->setSampleRate(getSampleRate());
    if (harEnabled) {
      for (int i = 0; i < cntHar; i++) {
        delete harOsc[i];
        harOsc[i] = selectOsc((int)newValue);
        harOsc[i]->setSampleRate(getSampleRate());
      }
    }
  } else if (parameterID == "carrOscType") {
    delete carrOsc;
    carrOsc = selectOsc((int)newValue);
    carrOsc->setSampleRate(getSampleRate());
  } else if (parameterID == "LFO1Type") {
    auto f = LFO1->getDefFreq();
    delete LFO1;
    LFO1 = selectOsc((int)newValue);
    LFO1->setSampleRate(getSampleRate());
    LFO1->setDefFreq(f);
  } else if (parameterID == "LFO1Freq") {
    LFO1->setDefFreq(newValue);
  }
}
void SynthVoice::setHar(bool enabled) {
  // No change
  if (enabled == harEnabled) return;
  if (enabled) {  // Enable it
    harEnabled = true;
    for (int i = 0; i < cntHar; i++) {
      delete harOsc[i];
      harOsc[i] = selectOsc((int)(*midiOscType));
      harOsc[i]->setSampleRate(getSampleRate());
    }
  } else {  // Disable it
    harEnabled = false;
    for (int i = 0; i < cntHar; i++) delete harOsc[i];
  }
}
