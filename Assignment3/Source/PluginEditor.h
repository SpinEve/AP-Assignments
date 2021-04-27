/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"
#include "Listener.h"

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
  float getVolume();
 private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  Assignment3AudioProcessor& audioProcessor;
  juce::Slider midiVolume;
  SliderListener* volumeListener;
  float volume;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Assignment3AudioProcessorEditor)
};
