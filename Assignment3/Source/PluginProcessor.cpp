/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"

#include "PluginEditor.h"
#include "Synth.h"

/// Create Parameter layout for ValueTreeState
juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
//==============================================================================
Assignment3AudioProcessor::Assignment3AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
              )
#endif
      ,
      parameters(*this, nullptr, juce::Identifier("Assignment_3"),
                 createParameterLayout()) {

  carrFreq = parameters.getRawParameterValue("carrFreq");
  noiseLevel = parameters.getRawParameterValue("noiseLevel");
  gain = parameters.getRawParameterValue("gain");
  moduType = parameters.getRawParameterValue("moduType");
  midiOscType = parameters.getRawParameterValue("midiOscType");
  carrOscType = parameters.getRawParameterValue("carrOscType");

  // ADSR
  attack = parameters.getRawParameterValue("attack");
  decay = parameters.getRawParameterValue("decay");
  sustain = parameters.getRawParameterValue("sustain");
  release = parameters.getRawParameterValue("release");

  // LFO1
  LFO1Type = parameters.getRawParameterValue("LFO1Type");
  LFO1Freq = parameters.getRawParameterValue("LFO1Freq");
  LFO1Amp = parameters.getRawParameterValue("LFO1Amp");
  LFO1Modu = parameters.getRawParameterValue("LFO1Modu");

  // Harmonics Gain
  harGain = parameters.getRawParameterValue("harGain");

  // Encoder
  encodeEnabled = parameters.getRawParameterValue("encodeEnabled");

  // Add SynthVoice and SynthSound
  for (auto i = 0; i < countVoice; i++) {
    synth.addVoice(new SynthVoice(parameters));
  }
  synth.addSound(new SynthSound());
}

Assignment3AudioProcessor::~Assignment3AudioProcessor() {}

//==============================================================================
const juce::String Assignment3AudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool Assignment3AudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool Assignment3AudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool Assignment3AudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double Assignment3AudioProcessor::getTailLengthSeconds() const { return 0.0; }

int Assignment3AudioProcessor::getNumPrograms() {
  return 1;  // NB: some hosts don't cope very well if you tell them there are 0
             // programs, so this should be at least 1, even if you're not
             // really implementing programs.
}

int Assignment3AudioProcessor::getCurrentProgram() { return 0; }

void Assignment3AudioProcessor::setCurrentProgram(int index) {}

const juce::String Assignment3AudioProcessor::getProgramName(int index) {
  return {};
}

void Assignment3AudioProcessor::changeProgramName(int index,
                                                  const juce::String& newName) {
}

//==============================================================================
void Assignment3AudioProcessor::prepareToPlay(double sampleRate,
                                              int samplesPerBlock) {
  sr = sampleRate;
  synth.setCurrentPlaybackSampleRate(sr);
}

void Assignment3AudioProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Assignment3AudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  // Some plugin hosts, such as certain GarageBand versions, will only
  // load plugins that support stereo bus layouts.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}
#endif

void Assignment3AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                             juce::MidiBuffer& midiMessages) {
  // Just give it to Synth
  synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool Assignment3AudioProcessor::hasEditor() const {
  return true;  // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Assignment3AudioProcessor::createEditor() {
  return new Assignment3AudioProcessorEditor(*this, parameters);
}

//==============================================================================
/** Save state of parameter to destData in xml format.
 *  Code reference to JUCE tutorial
 */
void Assignment3AudioProcessor::getStateInformation(
    juce::MemoryBlock& destData) {
  auto state = parameters.copyState();
  std::unique_ptr<juce::XmlElement> tmpXml(state.createXml());
  copyXmlToBinary(*tmpXml, destData);
}
/** Load state from data (xml) to parameters
 */
void Assignment3AudioProcessor::setStateInformation(const void* data,
                                                    int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xmlState(
      getXmlFromBinary(data, sizeInBytes));

  if (xmlState.get() != nullptr)
    if (xmlState->hasTagName(parameters.state.getType()))
      parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}
void Assignment3AudioProcessor::setEncodeText(juce::String s) {
  for (int i = 0; i < countVoice; i++) {
    SynthVoice* sv = dynamic_cast<SynthVoice*>(synth.getVoice(i));
    sv->setEncodeText(s);
  }
}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new Assignment3AudioProcessor();
}
juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout layout;

  // MIDI
  layout.add(std::make_unique<juce::AudioParameterInt>("midiOscType",
                                                       "MIDI Type", 1, 6, 1));
  // Carrier
  layout.add(std::make_unique<juce::AudioParameterInt>(
      "carrOscType", "Carrier Type", 1, 6, 1));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      "carrFreq", "Carrier Frequency",
      juce::NormalisableRange(440.f / 8, 440.f * 8, 1.f, 0.315f), 440.f));
  // Noise
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      "noiseLevel", "Noise Level", 0.f, 1.f, 0.f));
  // Modulation
  layout.add(std::make_unique<juce::AudioParameterInt>(
      "moduType", "Modulation Type", 1, 4, 1));

  // Gain
  layout.add(std::make_unique<juce::AudioParameterFloat>("gain", "Gain", 0.f,
                                                         1.f, 0.5f));
  // ADSR
  layout.add(std::make_unique<juce::AudioParameterFloat>("attack", "Attack",
                                                         0.f, 4.f, 0.1f));
  layout.add(std::make_unique<juce::AudioParameterFloat>("decay", "Decay", 0.f,
                                                         4.f, 0.1f));
  layout.add(std::make_unique<juce::AudioParameterFloat>("sustain", "Sustain",
                                                         0.f, 4.f, 1.0f));
  layout.add(std::make_unique<juce::AudioParameterFloat>("release", "Release",
                                                         0.f, 4.f, 0.5f));

  // LFO1
  layout.add(std::make_unique<juce::AudioParameterInt>("LFO1Type", "LFO1 Type",
                                                       1, 6, 1));
  layout.add(std::make_unique<juce::AudioParameterInt>(
      "LFO1Modu", "LFO1 Modulation", 1, 4, 1));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      "LFO1Freq", "LFO1 Freq", juce::NormalisableRange(0.1f, 10.f, 0.01f, 0.3f),
      1.f));  // Skewed range from precise control
  layout.add(std::make_unique<juce::AudioParameterFloat>("LFO1Amp", "LFO1 Amp",
                                                         0.f, 1.f, 0.5f));

  // Harmonics
  layout.add(std::make_unique<juce::AudioParameterInt>(
      "harType", "Harmonics Preset", 1, 4, 1));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      "harGain", "Harmonics Gain", 0.f, 1.f, 0.5f));

  // Encoder
  layout.add(std::make_unique<juce::AudioParameterBool>(
      "encodeEnabled", "Encode Enable", false));
  return layout;
}