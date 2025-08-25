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
    
private:
    TingeAudioProcessor& audioProcessor;
    
public:
    juce::AudioProcessorValueTreeState apvts;

    std::array<std::unique_ptr<ParameterInstance>, 8>
    rate, phase, weight;
    
    std::unique_ptr<ParameterInstance>
    nudgeForward, nudgeBackward, nudgeStrength, brake, brakeStrength, overlap, reset, resetMode;
    


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

