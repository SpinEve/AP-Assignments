/*
  ==============================================================================

    Pause.h
    Created: 1 Mar 2021 2:18:30pm
    Author:  Mallow

  ==============================================================================
*/

#pragma once
#include <cmath>
#include <ctime>

class Pause {
 private:
  int sampleNum, countSample;  // Length of playing samples and counters
  int pauseNum, countPause;    // Length of pause and counters
  float response = 1.0f;       // Response value
  bool status;                 // True: Playing, False: Pause

 public:
  Pause(int _sampleNum, int _pauseNum) {
    sampleNum = _sampleNum;
    pauseNum = _pauseNum;
    countSample = countPause = 0;
    status = true;
  }
  // Set play length
  void setSamplesNum(int _sampleNum) {
    sampleNum = _sampleNum;
    countSample = 0;
  }
  // Set pause length
  void setPauseNum(int _pauseNum) {
    pauseNum = _pauseNum;
    countPause = 0;
  }
  virtual void changePauseNum() { return; }  // Do nothing here now
  virtual float loop(float sample) {
    if (status) {
      countSample++;
      if (countSample >= sampleNum) {  // Play -> pause
        changePauseNum();
        countPause = 0;
        response = 0.1f;  // Keep a low volume
        status = false;
      }
    } else {
      countPause++;
      if (countPause >= pauseNum) {  // Pause -> play
        countSample = 0;
        response = 1.0f;
        status = true;
      }
    }
    return sample * response;
  }
};

/**
 * Random pause: pause random time from min to max
 */
class RandPause : public Pause {
 private:
  int minPauseNum, maxPauseNum;  // Minimum and maximum of pause length

 public:
  RandPause(int _sampleNum, int _minSN, int _maxSN)
      : Pause(_sampleNum, _minSN) {
    minPauseNum = _minSN;
    maxPauseNum = _maxSN;
    srand(time(0));
  }
  // Randomly set pause length after playing
  void changePauseNum() {
    setPauseNum(minPauseNum + rand() % (maxPauseNum - minPauseNum));
  }
};