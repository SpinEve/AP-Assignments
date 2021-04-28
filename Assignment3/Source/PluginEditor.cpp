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
  defCarrFreq = 440.f;
  carrFreqListener = new SliderListener(&carrFreq);  // TODO: function pointer
  carrFreqSlider.setRange(defCarrFreq, defCarrFreq * 4, 1.f);
  carrFreqSlider.setValue(defCarrFreq);
  carrFreqSlider.setDoubleClickReturnValue(true, defCarrFreq);
  
  carrFreqSlider.addListener(carrFreqListener);
  addAndMakeVisible(carrFreqSlider);
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
  auto Left = 20;
  carrFreqSlider.setBounds(Left, 20, getWidth() - Left - 10, 20);
}

float Assignment3AudioProcessorEditor::getCarrFreq() { return carrFreq; }
void Assignment3AudioProcessorEditor::setCarrFreq(float _carrFreq) {
  carrFreq = _carrFreq;
  carrFreqSlider.setValue(carrFreq);
}
