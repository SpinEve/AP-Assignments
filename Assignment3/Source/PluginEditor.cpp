/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"

#include "PluginProcessor.h"

//==============================================================================
Assignment3AudioProcessorEditor::Assignment3AudioProcessorEditor(
    Assignment3AudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize(400, 300);
}

Assignment3AudioProcessorEditor::~Assignment3AudioProcessorEditor() {}

//==============================================================================
void Assignment3AudioProcessorEditor::paint(juce::Graphics& g) {
  // (Our component is opaque, so we must completely fill the background with a
  // solid colour)
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

  g.setColour(juce::Colours::white);
  g.setFont(15.0f);
  g.drawFittedText("Hello World!", getLocalBounds(),
                   juce::Justification::centred, 1);
  midiVolume.setSliderStyle(juce::Slider::LinearBarVertical);
  midiVolume.setRange(0.0, 127.0, 1.0);
  midiVolume.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
  midiVolume.setPopupDisplayEnabled(true, false, this);
  midiVolume.setTextValueSuffix(" Volume");
  midiVolume.setValue(1.0);

  addAndMakeVisible (&midiVolume);
}

void Assignment3AudioProcessorEditor::resized() {
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..
  midiVolume.setBounds(40, 30, 20, getHeight() - 60);
}
