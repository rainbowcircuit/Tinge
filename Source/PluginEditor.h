/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Graphics.h"
#include "UserInterfaceLayout.h"
#include "InteractionLogic.h"
//==============================================================================
/**
*/
class TingeAudioProcessorEditor  : public juce::AudioProcessorEditor, juce::Timer
{
public:
    TingeAudioProcessorEditor (TingeAudioProcessor&, std::atomic<std::array<float, 3>>& phases, std::atomic<std::array<float, 16>>& noteValues);
    ~TingeAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    //==============================================================================
    void timerCallback() override;
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    std::atomic<std::array<float, 3>>& phasesAtomic;
    std::array<float, 3> phases;

    std::atomic<std::array<float, 16>>& noteValuesAtomic;

    juce::Slider rate1Slider, rate2Slider, rate3Slider, division1Slider, division2Slider, division3Slider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
    rate1Attachment, rate2Attachment, rate3Attachment, division1Attachment, division2Attachment, division3Attachment;
    
    juce::TextButton state1Button, state2Button, state3Button;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
    state1Attachment, state2Attachment, state3Attachment;

    
    SpinnerGraphics spinnerGraphics;

    std::unique_ptr<RotationLayout> rotationLayout1, rotationLayout2, rotationLayout3;
    
    
    TingeAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TingeAudioProcessorEditor)
};
