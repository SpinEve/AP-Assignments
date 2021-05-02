#include "Synth.h"

#include <JuceHeader.h>

#include "Encoder.h"

SynthVoice::SynthVoice(juce::AudioProcessorValueTreeState& vts)
    : valueTreeState(vts) {
  // Get pointers to parameters from valueTreeState.
  // and add listeners to it if more actions is needed.
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

  harGain = valueTreeState.getRawParameterValue("harGain");
  harType = valueTreeState.getRawParameterValue("harType");
  valueTreeState.addParameterListener("harType", this);

  encodeEnabled = valueTreeState.getRawParameterValue("encodeEnabled");

  // Oscillators init
  auto sr = getSampleRate();  // Save sample rate
  carrOsc = new SinOsc();
  carrOsc->setSampleRate(sr);

  midiOsc = new SinOsc();
  midiOsc->setSampleRate(sr);

  LFO1 = new SinOsc();
  LFO1->setSampleRate(sr);

  env.setSampleRate(sr);

  enco.setSampleRate(sr);

  // Harmonics init
  for (int i = 0; i < cntHar; i++) {
    harOsc[i] = new SinOsc();
    harOsc[i]->setSampleRate(sr);
    harAmp[i] = 0.f;
    encoHarAmp[i] = 0.f;
  }
}
SynthVoice::~SynthVoice() {
  delete carrOsc;
  delete midiOsc;
}
void SynthVoice::startNote(int midiNoteNumber, float velocity,
                           juce::SynthesiserSound* sound,
                           int currentPitchWheelPosition) {
  auto freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
  // Set default freq of MIDI
  midiOsc->setDefFreq(freq);
  // and also harmonics
  for (int i = 0; i < cntHar; i++) harOsc[i]->setDefFreq(freq * (i + 2));

  // Envelopes on
  env.noteOn();
  playing = true;
  isOff = false;
}
void SynthVoice::stopNote(float velocity, bool allowTailOff) {
  env.noteOff();
  isOff = true;
}
/// Return result of sample on carrOsc with no, frequency, phase or amp
/// modulation.
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
      // LFO Modulation Part
      float LFOSample = LFO1->getNextSample() * (*LFO1Amp);
      currentSample = modulate(midiOsc, LFOSample, (int)(*LFO1Modu));

      // Harmonics Part
      float harSample = 0.f;
      if ((bool)(*encodeEnabled)) {  // Encoder enabled
        // Set additional encoder amplifier
        enco.setNextHar(encoHarAmp, cntHar);
        // Multiply it
        for (auto j = 0; j < cntHar; j++)
          harSample += modulate(harOsc[j], LFOSample, (int)(*LFO1Modu)) *
                       harAmp[j] * (1 + encoHarAmp[j]);
      } else {  // Otherwise
        for (auto j = 0; j < cntHar; j++)
          harSample +=
              modulate(harOsc[j], LFOSample, (int)(*LFO1Modu)) * harAmp[j];
      }
      currentSample += harSample * (*harGain) / 2;  // Keep harmonics gain low

      // Modulation Part
      // We don't want it output carrOsc sample if no modulation
      if ((*moduType) > 1)
        currentSample = modulate(carrOsc, currentSample, (int)(*moduType));

      // Noise Part
      currentSample = (2 * random.nextFloat() - 1) * (*noiseLevel) +
                      currentSample * (1.f - (*noiseLevel));

      // Envelope Part
      currentSample *= env.getNextSample();

      // Gain, halved so not too loud
      currentSample *= (*gain) / 2;

      // Render Part
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
/// To simplify selection of oscillators
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

// Sometimes more actions are needed when parameter changed...
void SynthVoice::parameterChanged(const juce::String& parameterID,
                                  float newValue) {
  if (parameterID == "carrFreq") {  // Carrier Freq, reset carrOsc
    carrOsc->setDefFreq(newValue);
  } else if (parameterID == "attack") {  // ADSR
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
    midiOsc = selectOsc((int)newValue);       // Change Osc Type
    midiOsc->setSampleRate(getSampleRate());  // Set sample rate
    // Don't forget harmonics!
    for (int i = 0; i < cntHar; i++) {
      delete harOsc[i];
      harOsc[i] = selectOsc((int)newValue);
      harOsc[i]->setSampleRate(getSampleRate());
    }
  } else if (parameterID == "carrOscType") {
    delete carrOsc;
    carrOsc = selectOsc((int)newValue);
    carrOsc->setSampleRate(getSampleRate());
  } else if (parameterID == "LFO1Type") {
    auto f = LFO1->getDefFreq();  // Save original freq...
    delete LFO1;
    LFO1 = selectOsc((int)newValue);
    LFO1->setSampleRate(getSampleRate());
    LFO1->setDefFreq(f);  // ...and reset it
  } else if (parameterID == "LFO1Freq") {
    LFO1->setDefFreq(newValue);
  } else if (parameterID == "harType") {
    int tmp = (int)newValue;
    for (auto i = 0; i < cntHar; i++) {
      if (tmp == 1)
        harAmp[i] = 0.f;  // No harmonics
      else if (tmp == 2)
        harAmp[i] = 1.f / (i + 1);  // 1/x
      else if (tmp == 3)
        harAmp[i] = 1.f / ((i + 1) * (i + 1));  // 1/x^2
      else if (tmp == 4)
        harAmp[i] = (float)(cntHar - i) / cntHar;  // Linear decrease
    }
  }
}
void SynthVoice::setEncodeText(juce::String s) { enco.setText(s); }
