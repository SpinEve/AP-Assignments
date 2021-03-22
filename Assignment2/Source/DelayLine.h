/*
  ==============================================================================

    DelayLine.h
    Created: 24 Feb 2021 3:41:23pm
    Author:  Mallow

  ==============================================================================
*/

#pragma once
#include <cstring>
class DelayLine {
 private:
  float* samples;  // Store delayed samples
  int delaySize, readHead, writeHead, delayTime;
  float feedback;  // Feedback in "read and write", see loop()
  void setdelaySize(int _delaySize) {
    delaySize = _delaySize;
    samples = new float[delaySize];
    // Cleaning arrays for reading at start
    std::fill(samples, samples + delaySize, 0.0f);
  }

 public:
  DelayLine(int _delaySize = 48000) {
    setdelaySize(_delaySize);
    setdelayTime(0);  // Default params
    setfeedback(0.1f);
    readHead = writeHead = 0;
  }
  void setdelayTime(int _delayTime) {
    delayTime = _delayTime;
    readHead = writeHead - delayTime;         // Reset readhead position
    if (readHead < 0) readHead += delaySize;  // Circular array
  }
  void setfeedback(float fb) {
    // Keep feedback in [0, 1]
    if (fb < 0)
      feedback = 0.f;
    else if (fb > 1)
      feedback = 1.f;
    else
      feedback = fb;
  }

  // Read and write
  float loop(float newSample) {
    float tmp = read();
    write((1 - feedback) * newSample + feedback * tmp);  // Feedback mixture
    return tmp;
  }
  // Read delayed samples
  float read() {
    float ret = samples[readHead++];
    if (readHead >= delaySize) readHead -= delaySize;
    return ret;
  }
  // Write new samples
  void write(float newSample) {
    samples[writeHead++] = newSample;
    if (writeHead >= delaySize) writeHead -= delaySize;
  }
  ~DelayLine() { delete samples; }
};