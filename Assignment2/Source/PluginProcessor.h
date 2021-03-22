/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "DelayLine.h"
#include "Instru.h"
#include "Modulation.h"
#include "Oscillator.h"
#include "Pause.h"

//==============================================================================
/**
 */
class Assignment2AudioProcessor : public juce::AudioProcessor {
 public:
  //==============================================================================
  Assignment2AudioProcessor();
  ~Assignment2AudioProcessor() override;

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

 private:
  float gain = 0.2f;  // Output gain
  float sr;           // Store sample rate

  juce::Random random;          // Juce random function
  juce::IIRFilter myIIRFilter;  // Juce IIRFilter

  DelayLine* mydelayLine;
  Oscillator *posXOsc, *posYOsc, *ROsc, *delayOsc, *IIROsc;  // Many oscillators
  std::vector<Instru*> instrus;  // Use pointer in vector for polymorphism
  AM* myAM;                      // Amplitude modulation pointer
  PM* myPM;                      // Phase modulation pointer
  RandPause* rp;                 // Random pause pointer
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Assignment2AudioProcessor)
};
