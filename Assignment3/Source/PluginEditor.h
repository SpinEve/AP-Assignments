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

  //==============================================================================
  /** A function to simplify slider initialization
   */
  void initSlider(juce::Slider& sld, float min, float max, float interVal,
                  float defVal);
  /** Click encode button, which will make encodeTextBox visible and set
   * encodeEnabled
   */
  void encodeButtonClicked();
  /** When encode textbox changed
   */
  void encodeTextChanged();

 private:
  Assignment3AudioProcessor& audioProcessor;
  juce::Label freqLabel, moduLabel, midiLabel, noiseLabel, carrLabel, gainLabel,
      ADSRLabel, LFO1TypeLabel, LFO1ModuLabel, LFO1SliderLabel,
      LFO1AmpSliderLabel, harLabel, harGainLabel;
  juce::Slider carrFreqSlider, noiseSlider, gainSlider, attackSlider,
      decaySlider, sustainSlider, releaseSlider, LFO1Slider, LFO1AmpSlider,
      harGainSlider;
  juce::ComboBox moduTypeBox, midiOscTypeBox, carrOscTypeBox, LFO1TypeBox,
      LFO1ModuTypeBox, harBox;
  juce::TextEditor encodeText;
  juce::ToggleButton encodeButton;
  juce::AudioProcessorValueTreeState& valueTreeState;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
      gainAttach, noiseLevelAttach, carrFreqAttach, attackAttach, decayAttach,
      sustainAttach, releaseAttach, LFO1FreqAttach, LFO1AmpAttach,
      harGainAttach;
  std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>
      moduTypeBoxAttach, midiOscTypeBoxAttach, carrOscTypeBoxAttach,
      LFO1TypeAttach, LFO1ModuAttach, harBoxAttach;
  std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
      encodeAttach;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Assignment3AudioProcessorEditor)
};
