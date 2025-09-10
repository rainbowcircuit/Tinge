/*
  ==============================================================================

    Parameters.h
    Created: 20 Aug 2025 2:49:31pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class TingeAudioProcessor;
class ParameterInstance;
class Parameters
{
public:
    Parameters(TingeAudioProcessor& p);
    
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    static juce::StringArray getSyncRateOptions()
    {
        return juce::StringArray {
            // negative values
            "-1/32", "-1/24", "-1/16",
            "-1/12", "-1/8", "-1/6", "-3/16",
            "-1/4", "-5/16", "-1/3", "-3/8",
            "-1/2", "-3/4", "-1",  "-2",
            "-3", "-4", "-6", "-8",
            
            // no motion
            "No Motion",
            
            // positive values
            "8", "6", "4", "3",
            "2", "1", "3/4", "1/2",
            "3/8", "1/3", "5/16", "1/4",
            "3/16", "1/6", "1/8", "1/12",
            "1/16", "1/24", "1/32"
        };
    }
    
private:
    TingeAudioProcessor& audioProcessor;
    
public:
    juce::AudioProcessorValueTreeState apvts;

    std::array<std::unique_ptr<ParameterInstance>, 3>
    rateFree, rateSync, rateMode, phase, curve, opacity;
    
    std::unique_ptr<ParameterInstance>
    nudgeForward,
    nudgeBackward,
    brake,
    jog,
    hold,
    overlap,
    reset, resetMode,
    valueSlew,
    thresholdMode,
    thresholdPhase,
    maxThreshold;
    
};

class ParameterInstance : public juce::AudioProcessorParameter::Listener, juce::AsyncUpdater
{
public:
    ParameterInstance(TingeAudioProcessor& p, Parameters& pm, juce::String paramID);
    
    //==============================================================================
    void parameterValueChanged (int /*maybe unused*/, float newValue) override;
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override {}
    void handleAsyncUpdate() override;
    void triggerUpdate();
    
    //==============================================================================
    float get() const noexcept;
    float getSafe() const noexcept;

private:
    float valueSafe;
    std::atomic<float> value;
    std::atomic<float> cachedValue;

    juce::String paramID;
    juce::RangedAudioParameter* rangedParam = nullptr;

    TingeAudioProcessor& audioProcessor;
    Parameters& param;
};

