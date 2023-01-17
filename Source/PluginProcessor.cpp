/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SpaceCrusherAudioProcessor::SpaceCrusherAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ) ,
state(*this, nullptr, juce::Identifier("params"), {
    std::make_unique<juce::AudioParameterFloat>(
                  "noiseAmt",
                  "Noise Amount",
                  juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
                  0.0f),
    std::make_unique<juce::AudioParameterFloat>(
                  "bitRate",
                  "Bit Rate",
                  juce::NormalisableRange<float>(1.0f, 50.0f, 1.0f),
                  1.0f),
    std::make_unique<juce::AudioParameterFloat>(
                  "bitDepth",
                  "Bit Depth",
                  juce::NormalisableRange<float>(2.0f, 32.0f, 1.0f),
                  32.0f),
    std::make_unique<juce::AudioParameterFloat>(
                  "crushMix",
                  "Crush Mix",
                  juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
                  1.0f),
    std::make_unique<juce::AudioParameterFloat>(
                  "roomSize",
                  "Room Size",
                  juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
                  0.5f),
    std::make_unique<juce::AudioParameterFloat>(
                  "damping",
                  "Damping",
                  juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
                  0.5f),
    std::make_unique<juce::AudioParameterFloat>(
                  "width",
                  "Width",
                  juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
                  1.0f),
    std::make_unique<juce::AudioParameterFloat>(
                  "verbMix",
                  "Verb Mix",
                  juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
                  0.33f),
})
#endif
{
}

SpaceCrusherAudioProcessor::~SpaceCrusherAudioProcessor()
{
}

//==============================================================================
const juce::String SpaceCrusherAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SpaceCrusherAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SpaceCrusherAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SpaceCrusherAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SpaceCrusherAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SpaceCrusherAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SpaceCrusherAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SpaceCrusherAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SpaceCrusherAudioProcessor::getProgramName (int index)
{
    return {};
}

void SpaceCrusherAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SpaceCrusherAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    for (int channel = 0; channel < getTotalNumInputChannels(); channel++) {
        crushers.add(BitCrusher());
    }
    verb.setSampleRate(getSampleRate());
    noiseParam = (float*)state.getRawParameterValue("noiseAmt");
    bitRateParam = (float*)state.getRawParameterValue("bitRate");
    bitDepthParam = (float*)state.getRawParameterValue("bitDepth");
    crushMixParam = (float*)state.getRawParameterValue("crushMix");
    roomSizeParam = (float*)state.getRawParameterValue("roomSize");
    dampingParam = (float*)state.getRawParameterValue("damping");
    widthParam = (float*)state.getRawParameterValue("width");
    verbMixParam = (float*)state.getRawParameterValue("verbMix");
    
}

void SpaceCrusherAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SpaceCrusherAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SpaceCrusherAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    noiseBuffer.setSize(totalNumOutputChannels, buffer.getNumSamples());
    dryOutputBuffer.setSize(totalNumOutputChannels, buffer.getNumSamples());
    wetOutputBuffer.setSize(totalNumOutputChannels, buffer.getNumSamples());
    
    // copy for processing
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        dryOutputBuffer.copyFrom(channel, 0, buffer.getReadPointer(channel), buffer.getNumSamples());
        wetOutputBuffer.copyFrom(channel, 0, buffer.getReadPointer(channel), buffer.getNumSamples());
    }
    
    // generate white noise
    noiseBuffer.clear();
    juce::Array<float> noise = noiseMaker.makeNoise(buffer.getNumSamples());
    
    
    // set level of noise
    juce::FloatVectorOperations::multiply(noise.getRawDataPointer(), *noiseParam / 5, buffer.getNumSamples());
    
    // make noise level dynamic with input
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
        juce::FloatVectorOperations::multiply(noise.getRawDataPointer(), wetOutputBuffer.getWritePointer(channel), buffer.getNumSamples());
    
    // add noise to noise buffer
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
        juce::FloatVectorOperations::add(noiseBuffer.getWritePointer(channel), noise.getRawDataPointer(), buffer.getNumSamples());
    
    // add noise to output buffer
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
        wetOutputBuffer.addFrom(channel, 0, noiseBuffer.getReadPointer(channel), buffer.getNumSamples());
    
    // create reverb wet signal in reverb buffer
    verbParams.roomSize = *roomSizeParam;
    verbParams.damping = *dampingParam;
    verbParams.wetLevel = 1.0f;
    verbParams.dryLevel = 0.0f;
    verbParams.width = *widthParam;
    verb.setParameters(verbParams);
    
    if (totalNumOutputChannels == 1) {
        verb.processMono(wetOutputBuffer.getWritePointer(0), buffer.getNumSamples());
    }
    else {
        verb.processStereo(wetOutputBuffer.getWritePointer(0), wetOutputBuffer.getWritePointer(1), buffer.getNumSamples());
    }
    
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = wetOutputBuffer.getWritePointer (channel);
        
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            BitCrusher* crusher = &crushers.getReference(channel);
            crusher->crushSample(channelData, i, *bitRateParam, *bitDepthParam, *crushMixParam);
        }
    }
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        juce::FloatVectorOperations::multiply(wetOutputBuffer.getWritePointer(channel), *verbMixParam, buffer.getNumSamples());
        juce::FloatVectorOperations::multiply(dryOutputBuffer.getWritePointer(channel), 1 - *verbMixParam, buffer.getNumSamples());
    }
    
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        buffer.copyFrom(channel, 0, dryOutputBuffer, channel, 0, buffer.getNumSamples());
        buffer.addFrom(channel, 0, wetOutputBuffer, channel, 0, buffer.getNumSamples());
    }
}

//==============================================================================
bool SpaceCrusherAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SpaceCrusherAudioProcessor::createEditor()
{
    return new SpaceCrusherAudioProcessorEditor (*this, state);
}

//==============================================================================
void SpaceCrusherAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SpaceCrusherAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SpaceCrusherAudioProcessor();
}
