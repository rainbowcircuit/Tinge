/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Graphics.h"
#include "LookAndFeel.h"
#include "UserInterfaceLayout.h"
#include "InteractionLogic.h"
//==============================================================================
/**
*/
class TingeAudioProcessorEditor  : public juce::AudioProcessorEditor, juce::Timer, juce::AudioProcessorParameter::Listener
{
public:
    TingeAudioProcessorEditor (TingeAudioProcessor&, std::atomic<std::array<float, 3>>& phases, std::atomic<std::array<float, 16>>& noteValues);
    ~TingeAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    //==============================================================================
    void timerCallback() override;
    void mouseDown(const juce::MouseEvent& event) override
    {
            if (isoView){
                isoEnterToggle.start();
                isoView = false;
            } else {
                isoExitToggle.start();
                isoView = true;
            }
    }
    
    void parameterValueChanged (int parameterIndex, float newValue) override;
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override {}
    void initializeParameter();
    
private:
    float overlap, nudgeStrength;
    std::array<float, 3> rateFree, rateSync, phase, ratio, colorIndex, opacity;
    std::array<bool, 3> state, rateMode;
    
    bool isoView = false;
    float animationValue;
    juce::VBlankAnimatorUpdater updater { this };
    
    juce::Animator isoEnterToggle = juce::ValueAnimatorBuilder {}
        .withEasing(juce::Easings::createEaseInOut())
        .withDurationMs(1800.0f)
        .withValueChangedCallback([this] (auto value){ // lambda function
            animationValue = 1.0f - value;
            repaint();
        })
        .build();
    
    juce::Animator isoExitToggle = juce::ValueAnimatorBuilder {}
        .withEasing(juce::Easings::createEaseInOut())
        .withDurationMs(1800.0f)
        .withValueChangedCallback([this] (auto value){ // lambda function
            animationValue = value;
            repaint();
        })
        .build();

    std::atomic<std::array<float, 3>>& phasesAtomic;
    std::array<float, 3> phases;

    std::atomic<std::array<float, 16>>& noteValuesAtomic;

    DialGraphics overlapLAF { 3 };
    juce::Slider overlapSlider, hueSlider, offsetSlider;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
    overlapAttachment;
    

    
    SpinnerGraphics spinnerGraphics;

    std::unique_ptr<RotationLayout> rotationLayout1, rotationLayout2, rotationLayout3;
    std::unique_ptr<GlobalControlLayout> globalLayout;
    std::unique_ptr<ValueAssignmentLayout> valueAssignmentLayout;
    TingeAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TingeAudioProcessorEditor)
};
