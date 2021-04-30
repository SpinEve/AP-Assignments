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
  void carrOscTypeBoxChanged();
  void noiseSliderChanged();
  void gainSliderChanged();
  void ADSRChanged();
  void initSlider(juce::Slider& sld, float min, float max, float interVal,
                  float defVal);
  void encodeButtonClicked();
  void encodeTextChanged();

 private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  Assignment3AudioProcessor& audioProcessor;
  juce::Label freqLabel, moduLabel, midiLabel, noiseLabel, carrLabel, gainLabel,
      ADSRLabel;
  juce::Slider carrFreqSlider, noiseSlider, gainSlider;
  juce::Slider attackSlider, delaySlider, sustainSlider, releaseSlider;
  juce::ComboBox moduTypeBox, midiOscTypeBox, carrOscTypeBox;
  juce::TextEditor encodeText;
  juce::ToggleButton encodeButton;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Assignment3AudioProcessorEditor)
};
