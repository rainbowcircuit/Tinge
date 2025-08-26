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
#include "InteractionLogic.h"
#include "PresetControl.h"
#include "Presets.h"
#include "SpinnerControl.h"
#include "MiscControl.h"
#include "GraphicsHelper.h"
#include "UserInterfaceLookAndFeel.h"
//==============================================================================
/**
*/
class PresetControlsLayout;

class TingeAudioProcessorEditor  : public juce::AudioProcessorEditor, juce::Button::Listener, juce::Timer, GraphicsHelper
{
public:
    TingeAudioProcessorEditor (TingeAudioProcessor&, std::atomic<std::array<float, 3>>& phases, std::atomic<std::array<float, 16>>& noteValues);
    ~TingeAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    //==============================================================================
    void timerCallback() override;
    
    void buttonClicked(juce::Button *b) override
    {
        if (b == &controlWindowToggle)
        {
            if (isoView){
                isoEnterToggle.start();
                isoView = false;
            } else {
                isoExitToggle.start();
                isoView = true;
            }
        }
    }
     
    void buttonStateChanged(juce::Button *) override {}
        
private:
    juce::LookAndFeel_V4 laf;
    juce::TextButton controlWindowToggle;
    
    bool isoView = false;
    float animationValue;
    
    juce::VBlankAnimatorUpdater updater { this };
    
    juce::Animator isoEnterToggle = juce::ValueAnimatorBuilder {}
        .withEasing(juce::Easings::createEaseInOut())
        .withDurationMs(1200.0f)
        .withValueChangedCallback([this] (auto value){ // lambda function
            animationValue = 1.0f - value;
            repaint();
        })
        .build();
    
    juce::Animator isoExitToggle = juce::ValueAnimatorBuilder {}
        .withEasing(juce::Easings::createEaseInOut())
        .withDurationMs(1200.0f)
        .withValueChangedCallback([this] (auto value){ // lambda function
            animationValue = value;
            repaint();
        })
        .build();
     
    std::atomic<std::array<float, 3>>& phasesAtomic;
    std::array<float, 3> phases;

    std::atomic<std::array<float, 16>>& noteValuesAtomic;

  //  DialGraphics overlapLAF { 3 };
    juce::Slider overlapSlider, hueSlider, offsetSlider;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
    overlapAttachment;
    

    
    SpinnerGraphics spinnerGraphics;
    std::unique_ptr<PresetControlsLayout> presetLayout;

    std::unique_ptr<SpinnerLayout> rotationLayout1, rotationLayout2, rotationLayout3;
    std::unique_ptr<GlobalControlsLayout> globalLayout;
    TingeAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TingeAudioProcessorEditor)
};

class controlWindowLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {

    }
};
