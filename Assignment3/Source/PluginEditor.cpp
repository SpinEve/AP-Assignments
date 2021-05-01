/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"

#include "PluginProcessor.h"
/// Init sliders
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
    Assignment3AudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), audioProcessor(p), valueTreeState(vts) {
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

  addAndMakeVisible(ADSRLabel);
  ADSRLabel.setText("ADSR Adjustment", juce::dontSendNotification);

  addAndMakeVisible(LFO1TypeLabel);
  LFO1TypeLabel.setText("LFO1 Type", juce::dontSendNotification);
  LFO1TypeLabel.attachToComponent(&LFO1TypeBox, true);

  addAndMakeVisible(LFO1ModuLabel);
  LFO1ModuLabel.setText("LFO1 Modulation", juce::dontSendNotification);
  LFO1ModuLabel.attachToComponent(&LFO1ModuTypeBox, true);

  addAndMakeVisible(LFO1SliderLabel);
  LFO1SliderLabel.setText("LFO1 Frequency", juce::dontSendNotification);
  LFO1SliderLabel.attachToComponent(&LFO1Slider, true);

  addAndMakeVisible(LFO1AmpSlider);
  LFO1AmpSliderLabel.setText("LFO1 Amp", juce::dontSendNotification);
  LFO1AmpSliderLabel.attachToComponent(&LFO1AmpSlider, true);

  // Sliders
  float defCarrFreq = 440.f;
  initSlider(carrFreqSlider, defCarrFreq / 8, defCarrFreq * 8, 1.f,
             defCarrFreq);
  carrFreqSlider.setSkewFactorFromMidPoint(defCarrFreq);
  carrFreqAttach.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
      valueTreeState, "carrFreq", carrFreqSlider));

  initSlider(noiseSlider, 0.f, 1.f, 0.01f, 0.f);
  noiseLevelAttach.reset(
      new juce::AudioProcessorValueTreeState::SliderAttachment(
          valueTreeState, "noiseLevel", noiseSlider));

  initSlider(gainSlider, 0.f, 1.f, 0.01f, 0.5f);
  gainAttach.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
      valueTreeState, "gain", gainSlider));

  initSlider(attackSlider, 0.f, 2.f, 0.01f, 0.1f);
  attackSlider.onValueChange = [this] { ADSRChanged(); };

  initSlider(delaySlider, 0.f, 2.f, 0.01f, 0.1f);
  delaySlider.onValueChange = [this] { ADSRChanged(); };

  initSlider(sustainSlider, 0.f, 2.f, 0.01f, 1.f);
  sustainSlider.onValueChange = [this] { ADSRChanged(); };

  initSlider(releaseSlider, 0.f, 2.f, 0.01f, 0.5f);
  releaseSlider.onValueChange = [this] { ADSRChanged(); };

  initSlider(LFO1Slider, 0.1f, 10.f, 0.01f, 1.f);
  LFO1Slider.setSkewFactorFromMidPoint(1.f);
  LFO1Slider.onValueChange = [this] { setLFO1(); };

  initSlider(LFO1AmpSlider, 0.f, 1.f, 0.01f, 0.5f);
  LFO1AmpSlider.onValueChange = [this] { setLFO1(); };

  // initSlider(osc2Slider, 0.f, 1.f, 0.01f, 0.1f);
  // osc2Slider.onValueChange = [this] { osc2SliderChanged(); };

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

  addAndMakeVisible(LFO1TypeBox);
  LFO1TypeBox.addItem("Sin", 1);
  LFO1TypeBox.addItem("Cos", 2);
  LFO1TypeBox.addItem("Tri", 3);
  LFO1TypeBox.addItem("Square", 4);
  LFO1TypeBox.addItem("Sawtooth", 5);
  LFO1TypeBox.onChange = [this] { setLFO1(); };

  addAndMakeVisible(LFO1ModuTypeBox);
  LFO1ModuTypeBox.addItem("None", 1);
  LFO1ModuTypeBox.addItem("FM", 2);
  LFO1ModuTypeBox.addItem("PM", 3);
  LFO1ModuTypeBox.addItem("AM", 4);
  LFO1ModuTypeBox.onChange = [this] { setLFO1(); };

  addAndMakeVisible(encodeButton);
  encodeButton.setButtonText("Text encoder");
  encodeButton.onClick = [this] { encodeButtonClicked(); };

  addAndMakeVisible(encodeText);
  encodeText.setInputRestrictions(32, "abcdefghijklmnopqrstuvwxyz ");
  encodeText.onTextChange = [this] { encodeTextChanged(); };
  encodeText.setVisible(false);
}

Assignment3AudioProcessorEditor::~Assignment3AudioProcessorEditor() {}

//==============================================================================
void Assignment3AudioProcessorEditor::paint(juce::Graphics& g) {
  // (Our component is opaque, so we must completely fill the background with a
  // solid colour)
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
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

  ADSRLabel.setBounds(20, 7 * vertIndent + 6 * h, getWidth() - leftIndent - 10,
                      h);
  auto adsrWidth = getWidth() / 4 - 15;
  attackSlider.setBounds(20, 8 * vertIndent + 7 * h, adsrWidth, h);
  sustainSlider.setBounds(20 + adsrWidth + 10, 8 * vertIndent + 7 * h,
                          adsrWidth, h);
  delaySlider.setBounds(20 + 2 * (adsrWidth + 10), 8 * vertIndent + 7 * h,
                        adsrWidth, h);
  releaseSlider.setBounds(20 + 3 * (adsrWidth + 10), 8 * vertIndent + 7 * h,
                          adsrWidth, h);
  LFO1TypeBox.setBounds(leftIndent, 9 * vertIndent + 8 * h,
                        getWidth() - leftIndent - 10, h);
  LFO1ModuTypeBox.setBounds(leftIndent, 10 * vertIndent + 9 * h,
                            getWidth() - leftIndent - 10, h);
  LFO1Slider.setBounds(leftIndent, 11 * vertIndent + 10 * h,
                       getWidth() - leftIndent - 10, h);
  LFO1AmpSlider.setBounds(leftIndent, 12 * vertIndent + 11 * h,
                          getWidth() - leftIndent - 10, h);
  encodeButton.setBounds(20, 13 * vertIndent + 12 * h,
                         getWidth() - leftIndent - 10, h);
  encodeText.setBounds(20, 14 * vertIndent + 13 * h,
                       getWidth() - leftIndent - 10, h);
}
void Assignment3AudioProcessorEditor::moduTypeBoxChanged() {
  audioProcessor.setModuType(moduTypeBox.getSelectedId());
}
void Assignment3AudioProcessorEditor::midiOscTypeBoxChanged() {
  audioProcessor.setMidiOscType(midiOscTypeBox.getSelectedId());
}
void Assignment3AudioProcessorEditor::carrOscTypeBoxChanged() {
  audioProcessor.setCarrOscType(carrOscTypeBox.getSelectedId());
}
void Assignment3AudioProcessorEditor::ADSRChanged() {
  audioProcessor.setADSR(attackSlider.getValue(), delaySlider.getValue(),
                         sustainSlider.getValue(), releaseSlider.getValue());
}
void Assignment3AudioProcessorEditor::encodeButtonClicked() {
  auto state = encodeButton.getToggleState();
  if (state) {
    encodeText.setVisible(true);
    midiOscTypeBox.setSelectedId(1);  // Sin oscillator for encoding
  } else {
    encodeText.setVisible(false);
    encodeText.clear();
  }
}
void Assignment3AudioProcessorEditor::encodeTextChanged() {
  if (encodeButton.getToggleState()) {
    audioProcessor.setEncodeText(encodeText.getText());
  }
}
void Assignment3AudioProcessorEditor::setLFO1() {
  audioProcessor.setLFO1(LFO1TypeBox.getSelectedId(),
                         LFO1ModuTypeBox.getSelectedId(), LFO1Slider.getValue(),
                         LFO1AmpSlider.getValue());
}
