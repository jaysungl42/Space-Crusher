/*
  ==============================================================================

    BitCrusher.h
    Created: 17 Jun 2022 1:47:45pm
    Author:  Jay Sung

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <math.h>

class BitCrusher {
public:
    BitCrusher();
    void crushSample(float *data, int sample, int bitRate, int bitDepth, float mix);
private:
};
