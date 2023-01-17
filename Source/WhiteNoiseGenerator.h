/*
  ==============================================================================

    WhiteNoiseGenerator.h
    Created: 17 Jun 2022 2:54:21pm
    Author:  Jay Sung

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class WhiteNoiseGenerator {
public:
    WhiteNoiseGenerator();
    juce::Array<float> makeNoise(int numSamples);
private:
};
