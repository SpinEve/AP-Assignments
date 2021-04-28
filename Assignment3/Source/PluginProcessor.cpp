/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"

#include "PluginEditor.h"
#include "Synth.h"

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
{
  for (auto i = 0; i < countVoice; i++) {
    synth.addVoice(new SynthVoice());
  }
  synth.addSound(new SynthSound());
  addParameter(carrFreq = new juce::AudioParameterFloat(
                   "carrFreq", "Carrier Frequency", 440.f, 1760.f, 440.f));
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
  Assignment3AudioProcessorEditor* ape =
      dynamic_cast<Assignment3AudioProcessorEditor*>(getActiveEditor());
  if (ape != nullptr) {
    if (*carrFreq != ape->getCarrFreq()) {
      *carrFreq = ape->getCarrFreq();
      for (int i = 0; i < countVoice; i++) {
        SynthVoice* sv = dynamic_cast<SynthVoice*>(synth.getVoice(i));
        sv->setCarrFreq(*carrFreq);
      }
    }
    if (moduType != ape->getModuType()) {
      moduType = ape->getModuType();
      for (int i = 0; i < countVoice; i++) {
        SynthVoice* sv = dynamic_cast<SynthVoice*>(synth.getVoice(i));
        sv->setModuType(moduType);
      }
    }
  }
  synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool Assignment3AudioProcessor::hasEditor() const {
  return true;  // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Assignment3AudioProcessor::createEditor() {
  return new Assignment3AudioProcessorEditor(*this);
}

//==============================================================================
void Assignment3AudioProcessor::getStateInformation(
    juce::MemoryBlock& destData) {
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.
  juce::MemoryOutputStream(destData, true).writeFloat(*carrFreq);
}

void Assignment3AudioProcessor::setStateInformation(const void* data,
                                                    int sizeInBytes) {
  *carrFreq =
      juce::MemoryInputStream(data, static_cast<size_t>(sizeInBytes), false)
          .readFloat();
  Assignment3AudioProcessorEditor* ape =
      dynamic_cast<Assignment3AudioProcessorEditor*>(getActiveEditor());
  if (ape != nullptr) {
    ape->setCarrFreq(*carrFreq);
  }
  // You should use this method to restore your parameters from this memory
  // block, whose contents will have been created by the getStateInformation()
  // call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new Assignment3AudioProcessor();
}
