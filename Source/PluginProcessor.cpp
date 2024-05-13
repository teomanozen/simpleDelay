/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
CircularBufferDelayAudioProcessor::CircularBufferDelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

CircularBufferDelayAudioProcessor::~CircularBufferDelayAudioProcessor()
{
}

//==============================================================================
const juce::String CircularBufferDelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CircularBufferDelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CircularBufferDelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CircularBufferDelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CircularBufferDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CircularBufferDelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CircularBufferDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CircularBufferDelayAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CircularBufferDelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void CircularBufferDelayAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void CircularBufferDelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    //auto delayBufferSize = 2 * (sampleRate + samplesPerBlock);
    auto delayBufferSize = 2 * (sampleRate * samplesPerBlock);
    mSampleRate = sampleRate;
    delayBuffer.setSize(getTotalNumInputChannels(),delayBufferSize);
    delayBuffer.clear(); // Clear the delay buffer to initialize it with zeros
    writePosition = 0; // Reset write position
}

void CircularBufferDelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CircularBufferDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void CircularBufferDelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    auto bufferLength =  buffer.getNumSamples();
    auto delayBufferLength = delayBuffer.getNumSamples();
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        const float* bufferData = buffer.getReadPointer(channel);
        const float* delayBufferData = delayBuffer.getReadPointer(channel);
        float* dryBuffer = buffer.getWritePointer(channel);
        
        fillDelayBuffer(channel, bufferLength, delayBufferLength, bufferData, delayBufferData);
        
        getFromDelayBuffer(buffer, channel, bufferLength, delayBufferLength, bufferData, delayBufferData,delaytime);
        //feedbackDelay(channel, bufferLength, delayBufferLength, dryBuffer);
    }
    //Testing
    //DBG ("Delay Buffer Size: " << delayBufferSize);
    //DBG ("Buffer Size: " << bufferSize);
    //DBG ("Write Position: " << writePosition);
    
    writePosition += bufferLength;
    writePosition %= delayBufferLength; // This ensures that writePosition always stays between 0 and delayBufferSize
}

void CircularBufferDelayAudioProcessor::fillDelayBuffer(int channel, const int bufferLength, const int delayBufferLength,
                                                        const float* bufferData, const float* delayBufferData)
{
    const float gain = 0.8;
    //Check to see if main buffer copies to delay buffer without needing to wrap...
    if (delayBufferLength > bufferLength + writePosition)  //if yes
    {
        //copy main buffer contents to delay buffer
        delayBuffer.copyFromWithRamp(channel, writePosition, bufferData, bufferLength, gain, gain);
    }
    else//if no
    {
        const int bufferRemaining = delayBufferLength - writePosition;
        
        //Copy that amount of contents to the end...
        delayBuffer.copyFromWithRamp(channel, writePosition, bufferData, bufferRemaining, gain, gain);
        
        // Copy remaining amount to beginning of delay buffer.
        delayBuffer.copyFromWithRamp(channel, 0, bufferData, bufferLength - bufferRemaining, gain, gain); //--> old
        //delayBuffer.copyFromWithRamp(channel, 0, bufferData + bufferRemaining, bufferLength - bufferRemaining, gain, gain);
    }
    
}

void CircularBufferDelayAudioProcessor::getFromDelayBuffer(juce::AudioBuffer<float>& buffer, int channel, const int bufferLength ,const int delayBufferLength, const float* bufferData, const float* delayBufferData, int delaytime)
{
    int delayTime = delaytime;
    const int readPosition = static_cast<int> (delayBufferLength + writePosition - (mSampleRate * delayTime / 1000)) % delayBufferLength; // -->old
    //const int readPosition = (writePosition - static_cast<int>(mSampleRate * delayTime / 1000) + delayBufferLength) % delayBufferLength;

    
    if ( delayBufferLength > bufferLength + readPosition)
    {
        //buffer.addFrom ---> This add delay signal to main signal
        //buffer.copyFrom ---> This takes delay signal only
        buffer.copyFrom(channel, 0, delayBufferData + readPosition, bufferLength); // This takes delay signal only
    }
    else
    {
        const int bufferRemaining = delayBufferLength - readPosition;
        buffer.copyFrom(channel, 0, delayBufferData + readPosition, bufferRemaining);
        buffer.copyFrom(channel, bufferRemaining, delayBufferData, bufferLength - bufferRemaining);
    }
   
}

void CircularBufferDelayAudioProcessor::feedbackDelay(int channel, const int bufferLength ,const int delayBufferLength, float* dryBuffer)
{
    if ( delayBufferLength > bufferLength + writePosition)
    {
        delayBuffer.addFromWithRamp(channel, writePosition, dryBuffer, bufferLength, 0.8, 0.8);
    }
    else
    {
        const int bufferRemaining = delayBufferLength - writePosition;
        
        delayBuffer.addFromWithRamp(channel, bufferRemaining, dryBuffer, bufferRemaining, 0.8, 0.8);
        delayBuffer.addFromWithRamp(channel, 0, dryBuffer, bufferLength - bufferRemaining, 0.8, 0.8); //--> old
        //delayBuffer.addFromWithRamp(channel, 0, dryBuffer + bufferRemaining, bufferLength - bufferRemaining, 0.8, 0.8);
    }
}

//==============================================================================
bool CircularBufferDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CircularBufferDelayAudioProcessor::createEditor()
{
    return new CircularBufferDelayAudioProcessorEditor (*this);
  
}

//==============================================================================
void CircularBufferDelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CircularBufferDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CircularBufferDelayAudioProcessor();
}
