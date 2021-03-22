/*
  ==============================================================================

    Util.h
    Created: 1 Mar 2021 1:07:19pm
    Author:  Mallow
    Intro: Some util functions

  ==============================================================================
*/

#pragma once
#include <cmath>

// Note to frequency
float noteToFreq(float d) { return 440.f * pow(2, (d - 69) / 12); }

// Frequency to note
float freqToNote(float f) { return 12 * log2(f / 440.f) + 69; }

// Sigmoid function
float sigmoid(float k) { return 1 / (1 + exp(k)); }