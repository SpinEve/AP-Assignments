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
  setSize(800, 600);

  addAndMakeVisible(freqLabel);
  freqLabel.setText("Carrier Frequency", juce::dontSendNotification);
  addAndMakeVisible(moduLabel);
  moduLabel.setText("Modulation Method", juce::dontSendNotification);
  addAndMakeVisible(midiLabel);
  midiLabel.setText("MIDI Type", juce::dontSendNotification);

  float defCarrFreq = 440.f;
  addAndMakeVisible(carrFreqSlider);
  carrFreqSlider.setRange(defCarrFreq, defCarrFreq * 4, 1.f);
  carrFreqSlider.setValue(defCarrFreq);
  carrFreqSlider.setDoubleClickReturnValue(true, defCarrFreq);
  carrFreqSlider.onValueChange = [this] { freqSliderChanged(); };

  addAndMakeVisible(moduTypeBox);
  moduTypeBox.addItem("None", 1);
  moduTypeBox.addItem("FM", 2);
  moduTypeBox.addItem("PM", 3);
  moduTypeBox.addItem("AM", 4);
  moduTypeBox.onChange = [this] { moduTypeBoxChanged(); };

  addAndMakeVisible(midiOscTypeBox);
  midiOscTypeBox.addItem("Sin", 1);
  midiOscTypeBox.addItem("Cos", 2);
  midiOscTypeBox.addItem("Tri", 3);
  midiOscTypeBox.addItem("Square", 4);
  midiOscTypeBox.addItem("Sawtooth", 5);
  midiOscTypeBox.addItem("Noise", 6);
  midiOscTypeBox.onChange = [this] { midiOscTypeBoxChanged(); };

  addAndMakeVisible(noiseSlider);
  noiseSlider.setRange(0.f, 1.f, 0.01f);
  noiseSlider.setValue(0.f);
  noiseSlider.setDoubleClickReturnValue(true, 0.f);
  noiseSlider.onValueChange = [this] { noiseSliderChanged(); };
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

  midiLabel.setBounds(indent, indent, getWidth() - 2 * indent, h);
  midiOscTypeBox.setBounds(indent, 2 * indent + h, getWidth() - 2 * indent, h);

  moduLabel.setBounds(indent, 3 * indent + 2 * h, getWidth() - 2 * indent, h);
  moduTypeBox.setBounds(indent, 4 * indent + 3 * h, getWidth() - 2 * indent, h);

  freqLabel.setBounds(indent, 5 * indent + 4 * h, getWidth() - 2 * indent, h);
  carrFreqSlider.setBounds(indent, 6 * indent + 5 * h, getWidth() - 2 * indent,
                           h);

  noiseSlider.setBounds(indent, 7 * indent + 6 * h, getWidth() - 2 * indent, h);
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
void Assignment3AudioProcessorEditor::midiOscTypeBoxChanged() {
  int id = midiOscTypeBox.getSelectedId();
  if (id == 1)
    carrFreqSlider.setEnabled(false);
  else
    carrFreqSlider.setEnabled(true);
  audioProcessor.setMidiOscType(id);
}
void Assignment3AudioProcessorEditor::noiseSliderChanged() {
  audioProcessor.setNoiseLevel(noiseSlider.getValue());
}
