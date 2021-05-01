/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 */
class Assignment3AudioProcessor : public juce::AudioProcessor {
 public:
  //==============================================================================
  Assignment3AudioProcessor();
  ~Assignment3AudioProcessor() override;

  //==============================================================================
  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

  void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

  //==============================================================================
  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override;

  //==============================================================================
  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  //==============================================================================
  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String& newName) override;

  //==============================================================================
  void getStateInformation(juce::MemoryBlock& destData) override;
  void setStateInformation(const void* data, int sizeInBytes) override;
  void setModuType(int mt);
  void setMidiOscType(int ot);
  void setCarrOscType(int ot);
  void setLFO1(int type, int mt, float freq, float amp);
  void setEncodeText(juce::String s);

 private:
  //==============================================================================
  int countVoice, moduType, midiOscType, carrOscType;
  double sr;
  juce::Synthesiser synth;
  std::atomic<float>* carrFreq = nullptr;
  std::atomic<float>* noiseLevel = nullptr;
  std::atomic<float>* gain = nullptr;
  std::atomic<float>* attack = nullptr;
  std::atomic<float>* decay = nullptr;
  std::atomic<float>* sustain = nullptr;
  std::atomic<float>* release = nullptr;
  juce::AudioProcessorValueTreeState parameters;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Assignment3AudioProcessor)
};
