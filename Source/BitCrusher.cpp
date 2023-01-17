/*
  ==============================================================================

    BitCrusher.cpp
    Created: 17 Jun 2022 1:47:45pm
    Author:  Jay Sung

  ==============================================================================
*/


#include "BitCrusher.h"

BitCrusher::BitCrusher() {
}

void BitCrusher::crushSample(float *data, int sample, int bitRate, int bitDepth, float mix) {
    
    float dry = data[sample];
    // reduce bit depth
    float totalQLevel = powf(2, bitDepth);
    float remainder = fmodf(data[sample], 1 / totalQLevel);
    
    // quantize
    data[sample] = (mix * (data[sample] - remainder)) + ((1 - mix) * dry);
    
    if (bitRate > 1) {
        if (sample % bitRate != 0) {
            data[sample] = (mix * data[sample - (sample % bitRate)]) + ((1 - mix) * dry);
        }
    }
}
