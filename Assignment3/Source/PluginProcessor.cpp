/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"

#include "PluginEditor.h"
#include "Synth.h"

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
      parameters(*this, nullptr, juce::Identifier("Assignment 3"),
                 createParameterLayout()) {
  carrFreq = parameters.getRawParameterValue("carrFreq");
  noiseLevel = parameters.getRawParameterValue("noiseLevel");
  gain = parameters.getRawParameterValue("gain");
  moduType = parameters.getRawParameterValue("moduType");
  midiOscType = parameters.getRawParameterValue("midiOscType");
  carrOscType = parameters.getRawParameterValue("carrOscType");

  attack = parameters.getRawParameterValue("attack");
  decay = parameters.getRawParameterValue("decay");
  sustain = parameters.getRawParameterValue("sustain");
  release = parameters.getRawParameterValue("release");

  countVoice = 4;
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
void Assignment3AudioProcessor::getStateInformation(
    juce::MemoryBlock& destData) {
  auto state = parameters.copyState();
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

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
    // SynthVoice* sv = dynamic_cast<SynthVoice*>(synth.getVoice(i));
    // sv->setEncodeText(s);
  }
}
void Assignment3AudioProcessor::setLFO1(int type, int mt, float freq,
                                        float amp) {
  for (int i = 0; i < countVoice; i++) {
    SynthVoice* sv = dynamic_cast<SynthVoice*>(synth.getVoice(i));
    sv->setLFO1(type, mt, freq, amp);
  }
}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new Assignment3AudioProcessor();
}
juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout layout;
  layout.add(std::make_unique<juce::AudioParameterInt>("midiOscType",
                                                       "MIDI Type", 1, 6, 1));
  layout.add(std::make_unique<juce::AudioParameterInt>(
      "carrOscType", "Carrier Type", 1, 6, 1));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      "carrFreq", "Carrier Frequency",
      juce::NormalisableRange(440.f / 8, 440.f * 8, 1.f, 0.315f), 440.f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      "noiseLevel", "Noise Level", 0.f, 1.f, 0.f));
  layout.add(std::make_unique<juce::AudioParameterInt>(
      "moduType", "Modulation Type", 1, 4, 1));

  layout.add(std::make_unique<juce::AudioParameterFloat>("gain", "Gain", 0.f,
                                                         1.f, 0.5f));
  layout.add(std::make_unique<juce::AudioParameterFloat>("attack", "Attack",
                                                         0.f, 4.f, 0.1f));
  layout.add(std::make_unique<juce::AudioParameterFloat>("decay", "Decay", 0.f,
                                                         4.f, 0.1f));
  layout.add(std::make_unique<juce::AudioParameterFloat>("sustain", "Sustain",
                                                         0.f, 4.f, 1.0f));
  layout.add(std::make_unique<juce::AudioParameterFloat>("release", "Release",
                                                         0.f, 4.f, 0.5f));
  return layout;
}