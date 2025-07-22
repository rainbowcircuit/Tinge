/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MIDIProcessor.h"
//==============================================================================
/**
*/
class TingeAudioProcessor  : public juce::AudioProcessor, juce::AudioProcessorParameter::Listener
{
public:
    //==============================================================================
    TingeAudioProcessor();
    ~TingeAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts {
            *this, nullptr, "Parameters", createParameterLayout()
        };
        
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    //==============================================================================
    void parameterValueChanged (int parameterIndex, float newValue) override
    {
        juce::String newParameterID;
        float scaledValue = 0.0f;

        if (auto* param = dynamic_cast<juce::AudioProcessorParameterWithID*>(this->getParameters()[parameterIndex]))
        {
            if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(param))
            {
                scaledValue = rangedParam->convertFrom0to1(newValue);
                newParameterID = param->paramID;
            }
        }
        
        for (int i = 0 ; i < 3; i++){
            auto incr = juce::String(i);
            state[i] = juce::String("state") + incr == newParameterID ? scaledValue : state[i];
            rateFree[i] = juce::String("rateFree") + incr == newParameterID ? scaledValue : rateFree[i];
            rateSync[i] = juce::String("rateSync") + incr == newParameterID ? scaledValue : rateSync[i];
            rateMode[i] = juce::String("rateMode") + incr == newParameterID ? scaledValue : rateMode[i];
            phase[i] = juce::String("phase") + incr == newParameterID ? scaledValue : phase[i];
            division[i] = juce::String("division") + incr == newParameterID ? scaledValue : division[i];
            

        }


    }
    
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override {}

    void initializeParameters()
    {
        float globalNudgeForward = apvts.getRawParameterValue("globalNudgeForward")->load();
        float globalNudgeBackward = apvts.getRawParameterValue("globalNudgeBackward")->load();
        float globalBrake = apvts.getRawParameterValue("globalBrake")->load();
        float globalStrength = apvts.getRawParameterValue("globalStrength")->load();

        float noteScale = apvts.getRawParameterValue("noteScale")->load();
        
        float velocityScale = apvts.getRawParameterValue("velocityScale")->load();
        float controlScale = apvts.getRawParameterValue("controlScale")->load();

        float overlap = apvts.getRawParameterValue("overlap")->load();

            for(int index = 0; index < 3; index++)
            {
                juce::String stateID = "state" + juce::String(index);
                juce::String rateFreeID = "rateFree" + juce::String(index);
                juce::String rateSyncID = "rateSync" + juce::String(index);
                juce::String divisionID = "division" + juce::String(index);
                juce::String phaseID = "phase" + juce::String(index);
                juce::String opacityID = "opacity" + juce::String(index);

                float state = apvts.getRawParameterValue(stateID)->load();
                float rateFree = apvts.getRawParameterValue(rateFreeID)->load();
                float rateSync = apvts.getRawParameterValue(rateSyncID)->load();
                float division = apvts.getRawParameterValue(divisionID)->load();
                float opacity = apvts.getRawParameterValue(opacityID)->load();
                float phase = apvts.getRawParameterValue(phaseID)->load();
                
            }

    }
    
private:
    std::array<float, 3> rateFree, rateSync, phase, division, opacity;
    std::array<bool, 3> state, rateMode;
    
    
    
    std::array<float, 3> phases;
    std::atomic<std::array<float, 3>> phasesAtomic;

    std::array<float, 16> heldPitches;
    std::atomic<std::array<float, 16>> heldPitchesAtomic;
    
    std::array<Spinner, 3> rotation;
    MIDIProcessor midiProcessor;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TingeAudioProcessor)
};

