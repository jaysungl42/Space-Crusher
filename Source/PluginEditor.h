/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

//==============================================================================
/**
*/
class SpaceCrusherAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SpaceCrusherAudioProcessorEditor (SpaceCrusherAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~SpaceCrusherAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void addSlider(juce::String name, juce::String labelText, juce::Slider& slider, juce::Label& label, std::unique_ptr<SliderAttachment>& attachment);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SpaceCrusherAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& params;
    juce::Slider noiseSlider, bitRateSlider, bitDepthSlider, crushMixSlider, roomSizeSlider, dampingSlider, widthSlider, verbMixSlider;
    juce::Label noiseLabel, bitRateLabel, bitDepthLabel, crushMixLabel, roomSizeLabel, dampingLabel, widthLabel, verbMixLabel;
    std::unique_ptr<SliderAttachment> noiseAttachment, bitRateAttachment, bitDepthAttachment, crushMixAttachment, roomSizeAttachment, dampingAttachment, widthAttachment, verbMixAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpaceCrusherAudioProcessorEditor)
};
