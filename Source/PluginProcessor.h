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
        
        nudgeStrength = juce::String("nudgeStrength") == newParameterID ? scaledValue : nudgeStrength;
        nudgeForward = juce::String("nudgeForward") == newParameterID ? scaledValue :nudgeForward;
        nudgeBackward = juce::String("nudgeBackward") == newParameterID ? scaledValue : nudgeBackward;
        brake = juce::String("brake") == newParameterID ? scaledValue : brake;
        overlap = juce::String("overlap") == newParameterID ? scaledValue : overlap;

        
        for (int i = 0 ; i < 3; i++){
            auto incr = juce::String(i);
            
            state[i] = juce::String("state") + incr == newParameterID ? scaledValue : state[i];
            rateFree[i] = juce::String("rateFree") + incr == newParameterID ? scaledValue : rateFree[i];
            rateSync[i] = juce::String("rateSync") + incr == newParameterID ? scaledValue : rateSync[i];
            rateMode[i] = juce::String("rateMode") + incr == newParameterID ? scaledValue : rateMode[i];
            phase[i] = juce::String("phase") + incr == newParameterID ? scaledValue : phase[i];
            ratio[i] = juce::String("ratio") + incr == newParameterID ? scaledValue : ratio[i];
        }
    }
    
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override {}

    void initializeParameters()
    {
        nudgeStrength = apvts.getRawParameterValue("nudgeStrength")->load();
        nudgeForward = apvts.getRawParameterValue("nudgeForward")->load();
        nudgeBackward = apvts.getRawParameterValue("nudgeBackward")->load();
        brake = apvts.getRawParameterValue("brake")->load();
        overlap = apvts.getRawParameterValue("overlap")->load();

     //  float noteScale = apvts.getRawParameterValue("noteScale")->load();
        
     //   float velocityScale = apvts.getRawParameterValue("velocityScale")->load();
   //     float controlScale = apvts.getRawParameterValue("controlScale")->load();

        
            for(int index = 0; index < 3; index++)
            {
                juce::String stateID = "state" + juce::String(index);
                juce::String rateFreeID = "rateFree" + juce::String(index);
                juce::String rateSyncID = "rateSync" + juce::String(index);
                juce::String rateModeID = "rateMode" + juce::String(index);

                juce::String ratioID = "ratio" + juce::String(index);
                juce::String phaseID = "phase" + juce::String(index);
                juce::String opacityID = "opacity" + juce::String(index);

             //   float state = apvts.getRawParameterValue(stateID)->load();
                float rateFree = apvts.getRawParameterValue(rateFreeID)->load();
                float rateSync = apvts.getRawParameterValue(rateSyncID)->load();
                float rateMode = apvts.getRawParameterValue(rateModeID)->load();

                float ratio = apvts.getRawParameterValue(ratioID)->load();
          //      float opacity = apvts.getRawParameterValue(opacityID)->load();
                float phase = apvts.getRawParameterValue(phaseID)->load();
                
                rotation[index].nudge(nudgeStrength, nudgeForward, nudgeBackward, brake);
                rotation[index].setRate(rateSync, rateFree, rateMode, phase);
            }

    }
    
private:
    float nudgeStrength, nudgeForward, nudgeBackward, brake;
    int overlap;
    std::array<float, 3> rateFree, rateSync, phase, ratio, opacity;
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

