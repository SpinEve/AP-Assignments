/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"

//==============================================================================
/**
 */
class Assignment3AudioProcessorEditor : public juce::AudioProcessorEditor {
 public:
  Assignment3AudioProcessorEditor(Assignment3AudioProcessor&);
  ~Assignment3AudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics&) override;
  void resized() override;
  void setCarrFreq(float _carrFreq);
  void freqSliderChanged();
  void moduTypeBoxChanged();
  void midiOscTypeBoxChanged();
  void noiseSliderChanged();

 private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  Assignment3AudioProcessor& audioProcessor;
  juce::Label freqLabel, moduLabel, midiLabel, noiseLabel;
  juce::Slider carrFreqSlider, noiseSlider;
  juce::ComboBox moduTypeBox, midiOscTypeBox;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Assignment3AudioProcessorEditor)
};
