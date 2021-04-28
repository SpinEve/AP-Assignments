/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"

#include "Listener.h"
#include "PluginProcessor.h"

//==============================================================================
Assignment3AudioProcessorEditor::Assignment3AudioProcessorEditor(
    Assignment3AudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize(800, 600);

  float defCarrFreq = 440.f;
  addAndMakeVisible(carrFreqSlider);
  carrFreqSlider.setRange(defCarrFreq, defCarrFreq * 4, 1.f);
  carrFreqSlider.setValue(defCarrFreq);
  carrFreqSlider.setDoubleClickReturnValue(true, defCarrFreq);
  carrFreqSlider.onValueChange = [this] { freqSliderChanged(); };

  addAndMakeVisible(moduTypeBox);
  moduTypeBox.addItem("FM", 1);
  moduTypeBox.addItem("PM", 2);
  moduTypeBox.addItem("AM", 3);
  moduTypeBox.onChange = [this] { moduTypeBoxChanged(); };
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
}

void Assignment3AudioProcessorEditor::resized() {
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..
  auto indent = 10;
  auto h = 20;
  carrFreqSlider.setBounds(indent, indent, getWidth() - 2 * indent, h);
  moduTypeBox.setBounds(indent, 2 * indent + h, getWidth() - 2 * indent, h);
}
void Assignment3AudioProcessorEditor::setCarrFreq(float cf) {
  carrFreqSlider.setValue(cf);
}
void Assignment3AudioProcessorEditor::moduTypeBoxChanged() {
  audioProcessor.setModuType(moduTypeBox.getSelectedId());
}
void Assignment3AudioProcessorEditor::freqSliderChanged() {
  audioProcessor.setCarrFreq(carrFreqSlider.getValue());
}
