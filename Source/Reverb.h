/*
  ==============================================================================

    Reverb.h
    Created: 17 Jun 2022 1:47:59pm
    Author:  Jay Sung

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class Reverb {
public:
    Reverb();
    void verbSample(float roomSize, float damping, int bitDepth, float mix);
private:
    juce::Reverb verb;
};
