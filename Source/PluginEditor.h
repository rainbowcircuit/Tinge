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
#include "ThresholdControl.h"
#include "GraphicsHelper.h"
#include "UserInterfaceLookAndFeel.h"
#include "MiscGraphics.h"
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
    
    void mouseDown(const juce::MouseEvent &m) override
    {
        if (isoView){
            isoEnterToggle.start();
            isoView = false;
        } else {
            isoExitToggle.start();
            isoView = true;
        }
    }
    
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
        
        if(b == &spinnerControlToggle)
        {
            
            rotationLayout1->setVisible(true);
            rotationLayout2->setVisible(true);
            rotationLayout3->setVisible(true);
            thresholdLayout->setVisible(false);
            tabSelection = true;
        }
        
        if(b == &thresholdControlToggle)
        {
            rotationLayout1->setVisible(false);
            rotationLayout2->setVisible(false);
            rotationLayout3->setVisible(false);
            thresholdLayout->setVisible(true);
            tabSelection = false;
        }

    }
     
    void buttonStateChanged(juce::Button *) override {}
        
private:
    juce::Label overlapLabel;
    MiscLookAndFeel overlapLAF { MiscLAF::Overlap };
    juce::Slider overlapSlider;

    juce::TextButton controlWindowToggle,
    spinnerControlToggle,
    thresholdControlToggle;
    
    
    bool isoView = false, tabSelection = true;
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
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
    overlapAttachment;
    

    
    SpinnerGraphics spinnerGraphics;
    std::unique_ptr<PresetControlsLayout> presetLayout;

    std::unique_ptr<SpinnerLayout> rotationLayout1, rotationLayout2, rotationLayout3;
    std::unique_ptr<ThresholdLayout> thresholdLayout;

    std::unique_ptr<GlobalControlsLayout> globalLayout;
    TingeAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TingeAudioProcessorEditor)
};
