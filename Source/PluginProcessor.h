/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Parameters.h"
#include "MIDIProcessor.h"
//==============================================================================
/**
*/
class Parameters;
class TingeAudioProcessor  : public juce::AudioProcessor
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
    
    //==============================================================================
    std::unique_ptr<Parameters> params;

private:
    
    std::array<float, 3> phases;
    std::atomic<std::array<float, 3>> phasesAtomic;

    std::array<float, 16> heldPitches;
    std::atomic<std::array<float, 16>> heldPitchesAtomic;
    
    std::array<Spinner, 3> rotation;
    MIDIProcessor midiProcessor;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TingeAudioProcessor)
};

