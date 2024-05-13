/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CircularBufferDelayAudioProcessorEditor::CircularBufferDelayAudioProcessorEditor (CircularBufferDelayAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 200);
    
    delayTimeSlider.setSliderStyle (juce::Slider::Rotary);
    delayTimeSlider.setRange (0, 1000, 1);
    delayTimeSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 100, 10);
    delayTimeSlider.addListener(this);
    
    
    //feedbackSlider.setSliderStyle (juce::Slider::Rotary);
    //feedbackSlider.setRange (0, 100, 1);
    //feedbackSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 100, 10);
    //feedbackSlider.addListener(this);
    
    wetDrySlider.setSliderStyle (juce::Slider::Rotary);
    wetDrySlider.setRange (0, 100, 1);
    wetDrySlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 100, 10);
    wetDrySlider.addListener(this);
    
    // Inside the constructor
    feedbackToggleButton.setButtonText("Feedback On/Off");
    feedbackToggleButton.addListener(this); // Make sure your editor class inherits Button::Listener
    addAndMakeVisible(&feedbackToggleButton);

    
    addAndMakeVisible(&delayTimeSlider);
    addAndMakeVisible(&feedbackSlider);
    addAndMakeVisible(&wetDrySlider);
    
    // Create and configure the label
    delayTimeLabel.setText("Delay Time", juce::dontSendNotification);
    delayTimeLabel.attachToComponent(&delayTimeSlider, false);
    delayTimeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(&delayTimeLabel);
    
    
    //feedbackLabel.setText("Feedback", juce::dontSendNotification);
    //feedbackLabel.attachToComponent(&feedbackSlider, false);
    //feedbackLabel.setJustificationType(juce::Justification::centred);
    //addAndMakeVisible(&feedbackLabel);
    
    wetDryLabel.setText("Dry&Wet", juce::dontSendNotification);
    wetDryLabel.attachToComponent(&wetDrySlider, false);
    wetDryLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(&wetDryLabel);
}

CircularBufferDelayAudioProcessorEditor::~CircularBufferDelayAudioProcessorEditor()
{
}

//==============================================================================
void CircularBufferDelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
   // g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);

}

void CircularBufferDelayAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    delayTimeSlider.setBounds((getWidth() / 4 * 1) - (100 / 2), (getHeight() / 4 * 2) - (100 / 2), 100, 100);
    //feedbackSlider.setBounds((getWidth() / 4 * 2) - (100 / 2), (getHeight() / 4 * 2) - (100 / 2), 100, 100);
    wetDrySlider.setBounds((getWidth() / 4 * 2) - (100 / 2), (getHeight() / 4 * 2) - (100 / 2), 100, 100);
    
    feedbackToggleButton.setBounds((getWidth() / 4 * 3) - (100 / 2), (getHeight() / 4 * 2) - (100 / 2), 100, 100);

    
    
}

void CircularBufferDelayAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &delayTimeSlider)
    {
        audioProcessor.delaytime = delayTimeSlider.getValue();
    }
    if(slider == &feedbackSlider)
    {
        audioProcessor.feedback = feedbackSlider.getValue();
    }
    if(slider == &wetDrySlider)
    {
        audioProcessor.wetDry = wetDrySlider.getValue();
    }
}

void CircularBufferDelayAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &feedbackToggleButton)
    {
        audioProcessor.feedbackOnOff = feedbackToggleButton.getToggleState();
    }
}
