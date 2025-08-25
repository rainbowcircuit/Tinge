/*
  ==============================================================================

    Parameters.cpp
    Created: 20 Aug 2025 2:49:31pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#include "Parameters.h"

Parameters::Parameters(TingeAudioProcessor& p) : audioProcessor(p),
apvts(audioProcessor, nullptr, "Parameters", createParameterLayout())
{
    
    nudgeStrength = std::make_unique<ParameterInstance>(audioProcessor, *this, "nudgeStrength");
    nudgeForward = std::make_unique<ParameterInstance>(audioProcessor, *this, "nudgeForward");
    nudgeBackward = std::make_unique<ParameterInstance>(audioProcessor, *this, "nudgeBackward");
    brakeStrength = std::make_unique<ParameterInstance>(audioProcessor, *this, "brakeStrength");
    brake = std::make_unique<ParameterInstance>(audioProcessor, *this, "brake");
    reset = std::make_unique<ParameterInstance>(audioProcessor, *this, "reset");
    resetMode = std::make_unique<ParameterInstance>(audioProcessor, *this, "resetMode");
    
    overlap = std::make_unique<ParameterInstance>(audioProcessor, *this, "overlap");

    for (int i = 0; i < 3; i++){
        auto incr = juce::String(i);
        rate[i] = std::make_unique<ParameterInstance>(audioProcessor, *this, "rate" + incr);
        phase[i] = std::make_unique<ParameterInstance>(audioProcessor, *this, "phase" + incr);
        weight[i] = std::make_unique<ParameterInstance>(audioProcessor, *this, "weight" + incr);
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout
Parameters::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "nudgeStrength", 1},
                                                           "Nudge Strength",
                                                           juce::NormalisableRange<float> { 0.0f, 100.0f, 0.1 }, 100.0f));
    
    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID { "nudgeForward", 1},
                                                         "Nudge Forward",
                                                         0, 1, 0));

    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID { "nudgeBackward", 1},
                                                     "Nudge Backward",
                                                     0, 1, 0));

    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "brakeStrength", 1},
                                                           "Brake Strength",
                                                           juce::NormalisableRange<float> { 0.0f, 100.0f, 0.1 }, 100.0f));
    
    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID { "brake", 1},
                                                 "Brake",
                                                 0, 1, 0));
    
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID { "reset", 1},
                                                          "Reset",
                                                           false));

    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID { "resetMode", 1},
                                                            "Reset Mode", juce::StringArray { "Manual Reset", "Hold & Reset on Clear", "Hold on Clear" }, 0));

    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID { "overlap", 1},
                                                           "Overlap", 0, 6, 6));

        
    std::array<float, 3> phaseDefaults = { 0.0f, 33.0f, 66.0f };

    for(int rotation = 0; rotation < 3; rotation++)
    {
        juce::String rateFreeID = "rate" + juce::String(rotation);
        juce::String rateFreeName = "Rate" + juce::String(rotation + 1);
        
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { rateFreeID, 1},
                                                               rateFreeName,
                                                               juce::NormalisableRange<float> { -5.0f, 5.0f, 0.01 }, 0.0f));

        juce::String phaseID = "phase" + juce::String(rotation);
        juce::String phaseName = "Phase " + juce::String(rotation + 1);

        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { phaseID, 1},
                                                               phaseName,
                                                               juce::NormalisableRange<float> { 0.0f, 100.0f, 0.01 }, phaseDefaults[rotation]));

        juce::String weightID = "weight" + juce::String(rotation);
        juce::String weightName = "Weight " + juce::String(rotation + 1);
        
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { weightID, 1},
                                                               weightName,
                                                               juce::NormalisableRange<float> { 0.0, 1.00f, 0.01 }, 100.0f));
    }

    return layout;
}

ParameterInstance::ParameterInstance(TingeAudioProcessor& p, Parameters& pm, juce::String paramID) : audioProcessor(p), param(pm)
{
    this->paramID = paramID;
    
    float initValue = param.apvts.getRawParameterValue(paramID)->load();
    value.store(initValue);
    valueSafe = initValue;
    cachedValue.store(initValue);

    if (auto* parameter = dynamic_cast<juce::AudioProcessorParameterWithID*>(param.apvts.getParameter(paramID)))
    {
        if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*>(parameter))
        {
            rangedParam = ranged;
            rangedParam->addListener(this);
        }
    }
}

void ParameterInstance::parameterValueChanged (int /*maybe unused*/, float newValue)
{
    // load atomics for thread safe reading
    cachedValue.store(newValue);
    triggerUpdate();
    triggerAsyncUpdate();
}

void ParameterInstance::handleAsyncUpdate()
{
    if (rangedParam)
    {
        float newValue = cachedValue.load(std::memory_order_relaxed);
        
        if (auto* parameter = dynamic_cast<juce::AudioProcessorParameterWithID*>(param.apvts.getParameter(paramID)))
        {
            if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(parameter))
            {
                valueSafe = rangedParam->convertFrom0to1(newValue);
            }
        }
    }
}

void ParameterInstance::triggerUpdate()
{
    if (rangedParam)
    {
        float newValue = cachedValue.load(std::memory_order_relaxed);
        if (auto* parameter = dynamic_cast<juce::AudioProcessorParameterWithID*>(param.apvts.getParameter(paramID)))
        {
            if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(parameter))
            {
                value.store(rangedParam->convertFrom0to1(newValue));
            }
        }
    }
}

float ParameterInstance::get() const noexcept
{
    return value.load(std::memory_order_relaxed);
}

float ParameterInstance::getSafe() const noexcept
{
    return valueSafe;
}
