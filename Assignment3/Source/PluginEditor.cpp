/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"

#include "PluginProcessor.h"
void Assignment3AudioProcessorEditor::initSlider(juce::Slider& sld, float min,
                                                 float max, float interVal,
                                                 float defVal) {
  addAndMakeVisible(sld);
  sld.setRange(min, max, interVal);
  sld.setValue(defVal);
  sld.setDoubleClickReturnValue(true, defVal);
}
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
  initSlider(carrFreqSlider, defCarrFreq, defCarrFreq * 4, 1.f, defCarrFreq);
  carrFreqSlider.onValueChange = [this] { freqSliderChanged(); };

  initSlider(noiseSlider, 0.f, 1.f, 0.01f, 0.f);
  noiseSlider.onValueChange = [this] { noiseSliderChanged(); };

  initSlider(gainSlider, 0.f, 1.f, 0.01f, 0.5f);
  gainSlider.onValueChange = [this] { gainSliderChanged(); };

  initSlider(attackSlider, 0.f, 2.f, 0.01f, 0.1f);
  attackSlider.onValueChange = [this] { ADSRChanged(); };

  initSlider(delaySlider, 0.f, 2.f, 0.01f, 0.1f);
  delaySlider.onValueChange = [this] { ADSRChanged(); };

  initSlider(sustainSlider, 0.f, 2.f, 0.01f, 1.f);
  sustainSlider.onValueChange = [this] { ADSRChanged(); };

  initSlider(releaseSlider, 0.f, 2.f, 0.01f, 0.5f);
  releaseSlider.onValueChange = [this] { ADSRChanged(); };

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

  auto adsrWidth = getWidth() / 4 - 15;
  attackSlider.setBounds(20, 7 * vertIndent + 6 * h, adsrWidth, h);
  sustainSlider.setBounds(20 + adsrWidth + 10, 7 * vertIndent + 6 * h,
                          adsrWidth, h);
  delaySlider.setBounds(20 + 2 * (adsrWidth + 10), 7 * vertIndent + 6 * h,
                        adsrWidth, h);
  releaseSlider.setBounds(20 + 3 * (adsrWidth + 10), 7 * vertIndent + 6 * h,
                          adsrWidth, h);
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
void Assignment3AudioProcessorEditor::ADSRChanged() {
  audioProcessor.setADSR(attackSlider.getValue(), delaySlider.getValue(),
                         sustainSlider.getValue(), releaseSlider.getValue());
}
