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
  volumeListener = new SliderListener(&volume);
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

  midiVolume.setRange(0.f, 100.f);
  midiVolume.addListener(volumeListener);
  addAndMakeVisible(midiVolume);
}

void Assignment3AudioProcessorEditor::resized() {
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..
  auto Left = 20;
  midiVolume.setBounds(Left, 20, getWidth() - Left - 10, 20);
}

float Assignment3AudioProcessorEditor::getVolume() { return volume; }
