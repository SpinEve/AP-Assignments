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
  Assignment3AudioProcessorEditor(Assignment3AudioProcessor&,
                                  juce::AudioProcessorValueTreeState&);
  ~Assignment3AudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics&) override;
  void resized() override;
  void initSlider(juce::Slider& sld, float min, float max, float interVal,
                  float defVal);
  void encodeButtonClicked();
  void encodeTextChanged();
  void setLFO1();

 private:
  Assignment3AudioProcessor& audioProcessor;
  juce::Label freqLabel, moduLabel, midiLabel, noiseLabel, carrLabel, gainLabel,
      ADSRLabel, LFO1TypeLabel, LFO1ModuLabel, LFO1SliderLabel,
      LFO1AmpSliderLabel;
  juce::Slider carrFreqSlider, noiseSlider, gainSlider, attackSlider,
      decaySlider, sustainSlider, releaseSlider, LFO1Slider, LFO1AmpSlider;
  juce::ComboBox moduTypeBox, midiOscTypeBox, carrOscTypeBox, LFO1TypeBox,
      LFO1ModuTypeBox;
  juce::TextEditor encodeText;
  juce::ToggleButton encodeButton;
  juce::AudioProcessorValueTreeState& valueTreeState;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
      gainAttach, noiseLevelAttach, carrFreqAttach, attackAttach, decayAttach,
      sustainAttach, releaseAttach;
  std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>
      moduTypeBoxAttach, midiOscTypeBoxAttach, carrOscTypeBoxAttach;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Assignment3AudioProcessorEditor)
};
