/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SpaceCrusherAudioProcessorEditor::SpaceCrusherAudioProcessorEditor (SpaceCrusherAudioProcessor& p, juce::AudioProcessorValueTreeState& state)
    : AudioProcessorEditor (&p), audioProcessor (p), params(state)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addSlider("noiseAmt", "Noise Amount", noiseSlider, noiseLabel, noiseAttachment);
    addSlider("bitRate", "Bit Rate", bitRateSlider, bitRateLabel, bitRateAttachment);
    addSlider("bitDepth", "Bit Depth", bitDepthSlider, bitDepthLabel, bitDepthAttachment);
    addSlider("crushMix", "Crush Mix", crushMixSlider, crushMixLabel, crushMixAttachment);
    addSlider("roomSize", "Room Size", roomSizeSlider, roomSizeLabel, roomSizeAttachment);
    addSlider("damping", "Damping", dampingSlider, dampingLabel, dampingAttachment);
    addSlider("width", "Width", widthSlider, widthLabel, widthAttachment);
    addSlider("verbMix", "Verb Mix", verbMixSlider, verbMixLabel, verbMixAttachment);
    setSize (400, 400);
}

SpaceCrusherAudioProcessorEditor::~SpaceCrusherAudioProcessorEditor()
{
}

//==============================================================================
void SpaceCrusherAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void SpaceCrusherAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    noiseSlider.setBounds(100, 0, 200, 50);
    bitRateSlider.setBounds(100, 50, 200, 50);
    bitDepthSlider.setBounds(100, 100, 200, 50);
    crushMixSlider.setBounds(100, 150, 200, 50);
    roomSizeSlider.setBounds(100, 200, 200, 50);
    dampingSlider.setBounds(100, 250, 200, 50);
    widthSlider.setBounds(100, 300, 200, 50);
    verbMixSlider.setBounds(100, 350, 200, 50);
}


void SpaceCrusherAudioProcessorEditor::addSlider(juce::String name, juce::String labelText, juce::Slider& slider, juce::Label& label, std::unique_ptr<SliderAttachment>& attachment) {
   addAndMakeVisible(slider);
   attachment.reset(new SliderAttachment(params, name, slider));
    label.setText(labelText, juce::dontSendNotification);
   label.attachToComponent(&slider, true);
   addAndMakeVisible(label);
}
