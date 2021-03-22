/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"

#include "DelayLine.h"
#include "Instru.h"
#include "Modulation.h"
#include "Oscillator.h"
#include "PluginEditor.h"
#include "Pause.h"
#include "Util.h"

//==============================================================================
Assignment2AudioProcessor::Assignment2AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      )
#endif
{
}

Assignment2AudioProcessor::~Assignment2AudioProcessor() {}

//==============================================================================
const juce::String Assignment2AudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool Assignment2AudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool Assignment2AudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool Assignment2AudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double Assignment2AudioProcessor::getTailLengthSeconds() const { return 0.0; }

int Assignment2AudioProcessor::getNumPrograms() {
  return 1;  // NB: some hosts don't cope very well if you tell them there are 0
             // programs, so this should be at least 1, even if you're not
             // really implementing programs.
}

int Assignment2AudioProcessor::getCurrentProgram() { return 0; }

void Assignment2AudioProcessor::setCurrentProgram(int index) {}

const juce::String Assignment2AudioProcessor::getProgramName(int index) {
  return {};
}

void Assignment2AudioProcessor::changeProgramName(int index,
                                                  const juce::String& newName) {
}

//==============================================================================
void Assignment2AudioProcessor::prepareToPlay(double sampleRate,
                                              int samplesPerBlock) {
  // Store sample rate for using afterwards
  sr = sampleRate;

  //=====================Initialization===============================

  // Generate a major lowC chord
  instrus.push_back(new Instru(sr, noteToFreq(57)));
  instrus.push_back(new Instru(sr, noteToFreq(61)));
  instrus.push_back(new Instru(sr, noteToFreq(64)));

  // Init oscillators
  posXOsc = new CosOsc(sr, 0.02f);  // Cosine oscil for position X
  posYOsc = new SinOsc(sr, 0.02f);  // Sine oscil for position Y
  ROsc = new CosOsc(sr, 0.05f);     // Cos oscil for radius R

  delayOsc = new SinOsc(sr);      // Delay time oscillator
  IIROsc = new SinOsc(sr, 0.7f);  // IIR cycle = 10/7s

  myAM = new AM(sr, noteToFreq(63));  // Init modulations with carrier note 63

  // Pause for 0.25s, restart randomly from 4 to 8s
  rp = new RandPause(sr / 4, sr * 4, sr * 8);

  mydelayLine = new DelayLine(sr);       // Delay buffer size = 1s
  mydelayLine->setdelayTime(0.01 * sr);  // Delay 0.01s = 10 ms

  myIIRFilter.setCoefficients(
      juce::IIRCoefficients::makeLowPass(sr, 300.0f, 5.0f));
  myIIRFilter.reset();
}

void Assignment2AudioProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Assignment2AudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  // Some plugin hosts, such as certain GarageBand versions, will only
  // load plugins that support stereo bus layouts.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}
#endif

void Assignment2AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                             juce::MidiBuffer& midiMessages) {
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();
  int numSamples = buffer.getNumSamples();
  // Refreshing buffer
  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; i++) {
    buffer.clear(i, 0, numSamples);
  }

  auto* leftChannel = buffer.getWritePointer(0);
  auto* rightChannel = buffer.getWritePointer(1);

  for (int i = 0; i < numSamples; i++) {
    // Add chord together
    float sample = instrus[0]->loop() + instrus[1]->loop() + instrus[2]->loop();

    // Delay 10+-2.5ms for "flanging" effect
    mydelayLine->setdelayTime(0.01 * sr * (1 + delayOsc->loop() / 4));
    sample += mydelayLine->loop(sample);

    // Amplitude modulation (just for fun)
    sample = myAM->loop(sample);

    // LowPass Filter varies from 250~350Hz
    myIIRFilter.setCoefficients(juce::IIRCoefficients::makeLowPass(
        sr, 300.0f + IIROsc->loop() * 50.0f, 5.0f));
    sample = myIIRFilter.processSingleSampleRaw(sample);

    // Control sound "position"
    float R = 2 * (ROsc->loop() + 1) + 3;
    float posX = posXOsc->loop() * R;
    float posY = posYOsc->loop() * R;
    // Calculate distance^2 from left (-1, 0) and right (1, 0)
    float dLeft2 = (posX + 1) * (posX + 1) + posY * posY;
    float dRight2 = (posX - 1) * (posX - 1) + posY * posY;

    // Add random noises
    float noiseSample = random.nextFloat();
    noiseSample = 0.3 * rp->loop(noiseSample); // Lower down noise

    // Noise won't "move", keep it same in both channels
    leftChannel[i] = gain * (sample / dLeft2 + noiseSample);
    rightChannel[i] = gain * (sample / dRight2 + noiseSample);
  }
}

//==============================================================================
bool Assignment2AudioProcessor::hasEditor() const {
  return true;  // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Assignment2AudioProcessor::createEditor() {
  return new Assignment2AudioProcessorEditor(*this);
}

//==============================================================================
void Assignment2AudioProcessor::getStateInformation(
    juce::MemoryBlock& destData) {
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.
}

void Assignment2AudioProcessor::setStateInformation(const void* data,
                                                    int sizeInBytes) {
  // You should use this method to restore your parameters from this memory
  // block, whose contents will have been created by the getStateInformation()
  // call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new Assignment2AudioProcessor();
}
