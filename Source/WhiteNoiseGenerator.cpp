/*
  ==============================================================================

    WhiteNoiseGenerator.cpp
    Created: 17 Jun 2022 2:54:21pm
    Author:  Jay Sung

  ==============================================================================
*/

#include "WhiteNoiseGenerator.h"

WhiteNoiseGenerator::WhiteNoiseGenerator()
{
}

juce::Array<float> WhiteNoiseGenerator::makeNoise(int numSamples)
{
    juce::Array<float> noise;
        
    float z0 = 0;
    float z1 = 0;
    bool generate = false;
    
    float mu = 0; // center (0)
    float sigma = 1; // spread -1 <-> 1
    
    float output = 0;
    float u1 = 0;
    float u2 = 0;
    
    juce::Random r = juce::Random::getSystemRandom();
    r.setSeed(juce::Time::getCurrentTime().getMilliseconds());
    const float epsilon = std::numeric_limits<float>::min();
    
    for (int s=0; s < numSamples; s++)
    {
        
        // GENERATE ::::
        // using box muller method
        // https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
        generate = !generate;
        
        if (!generate)
            output =  z1 * sigma + mu;
        else
        {
            do
            {
                u1 = r.nextFloat();
                u2 = r.nextFloat();
            }
            while ( u1 <= epsilon );
            
            z0 = sqrtf(-2.0 * logf(u1)) * cosf(2 * juce::MathConstants<float>::pi * u2);
            z1 = sqrtf(-2.0 * logf(u1)) * sinf(2 * juce::MathConstants<float>::pi * u2);
            
            output = z0 * sigma + mu;
        }
        
        // NAN check ...
        jassert(output == output);
        jassert(output > -50 && output < 50);
        
        //
        noise.add(output);
        
    }
    
    return noise;
}
