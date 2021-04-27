#pragma once
#include <JuceHeader.h>

class SliderListener : public juce::Slider::Listener {
 public:
  SliderListener(float* _val) { val = _val; }
  void sliderValueChanged(juce::Slider* slider) override {
    (*val) = slider->getValue();
  }

 private:
  float* val;
};