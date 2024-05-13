/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class CircularBufferDelayAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener
{
public:
    CircularBufferDelayAudioProcessorEditor (CircularBufferDelayAudioProcessor&);
    ~CircularBufferDelayAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void sliderValueChanged(juce::Slider* slider) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CircularBufferDelayAudioProcessor& audioProcessor;
    
    juce::Slider delayTimeSlider;
    juce::Slider feedbackSlider;
    juce::Slider wetDrySlider;
    
    juce::Label delayTimeLabel;
    juce::Label feedbackLabel;
    juce::Label wetDryLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CircularBufferDelayAudioProcessorEditor)
    
};
