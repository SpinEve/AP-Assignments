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

  // Labels
  addAndMakeVisible(freqLabel);
  freqLabel.setText("Carrier Frequency", juce::dontSendNotification);
  freqLabel.attachToComponent(&carrFreqSlider, true);

  addAndMakeVisible(moduLabel);
  moduLabel.setText("Modulation Method", juce::dontSendNotification);
  moduLabel.attachToComponent(&moduTypeBox, true);

  addAndMakeVisible(midiLabel);
  midiLabel.setText("MIDI Type", juce::dontSendNotification);
  midiLabel.attachToComponent(&midiOscTypeBox, true);

  addAndMakeVisible(noiseLabel);
  noiseLabel.setText("Noise Level", juce::dontSendNotification);
  noiseLabel.attachToComponent(&noiseSlider, true);

  addAndMakeVisible(carrLabel);
  carrLabel.setText("Carrier Type", juce::dontSendNotification);
  carrLabel.attachToComponent(&carrOscTypeBox, true);

  addAndMakeVisible(gainLabel);
  gainLabel.setText("Gain", juce::dontSendNotification);
  gainLabel.attachToComponent(&gainSlider, true);

  // Sliders
  float defCarrFreq = 440.f;
  addAndMakeVisible(carrFreqSlider);
  carrFreqSlider.setRange(defCarrFreq, defCarrFreq * 4, 1.f);
  carrFreqSlider.setValue(defCarrFreq);
  carrFreqSlider.setDoubleClickReturnValue(true, defCarrFreq);
  carrFreqSlider.onValueChange = [this] { freqSliderChanged(); };

  addAndMakeVisible(noiseSlider);
  noiseSlider.setRange(0.f, 1.f, 0.01f);
  noiseSlider.setValue(0.f);
  noiseSlider.setDoubleClickReturnValue(true, 0.f);
  noiseSlider.onValueChange = [this] { noiseSliderChanged(); };

  addAndMakeVisible(gainSlider);
  gainSlider.setRange(0.f, 1.f, 0.01f);
  gainSlider.setValue(0.5f);
  gainSlider.setDoubleClickReturnValue(true, 0.5f);
  gainSlider.onValueChange = [this] { gainSliderChanged(); };

  // Boxes
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

  addAndMakeVisible(carrOscTypeBox);
  carrOscTypeBox.addItem("Sin", 1);
  carrOscTypeBox.addItem("Cos", 2);
  carrOscTypeBox.addItem("Tri", 3);
  carrOscTypeBox.addItem("Square", 4);
  carrOscTypeBox.addItem("Sawtooth", 5);
  carrOscTypeBox.onChange = [this] { carrOscTypeBoxChanged(); };
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
  auto leftIndent = 120;
  auto vertIndent = 10;
  auto h = 20;

  midiOscTypeBox.setBounds(leftIndent, vertIndent, getWidth() - leftIndent - 10,
                           h);
  moduTypeBox.setBounds(leftIndent, 2 * vertIndent + h,
                        getWidth() - leftIndent - 10, h);
  carrOscTypeBox.setBounds(leftIndent, 3 * vertIndent + 2 * h,
                           getWidth() - leftIndent - 10, h);
  carrFreqSlider.setBounds(leftIndent, 4 * vertIndent + 3 * h,
                           getWidth() - leftIndent - 10, h);
  noiseSlider.setBounds(leftIndent, 5 * vertIndent + 4 * h,
                        getWidth() - leftIndent - 10, h);
  gainSlider.setBounds(leftIndent, 6 * vertIndent + 5 * h,
                       getWidth() - leftIndent - 10, h);
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
  audioProcessor.setMidiOscType(midiOscTypeBox.getSelectedId());
}
void Assignment3AudioProcessorEditor::carrOscTypeBoxChanged() {
  audioProcessor.setCarrOscType(carrOscTypeBox.getSelectedId());
}
void Assignment3AudioProcessorEditor::noiseSliderChanged() {
  audioProcessor.setNoiseLevel(noiseSlider.getValue());
}
void Assignment3AudioProcessorEditor::gainSliderChanged() {
  audioProcessor.setGain(gainSlider.getValue());
}
