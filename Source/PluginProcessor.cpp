/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TingeAudioProcessor::TingeAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

TingeAudioProcessor::~TingeAudioProcessor()
{
}

//==============================================================================
const juce::String TingeAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TingeAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TingeAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TingeAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TingeAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TingeAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TingeAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TingeAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TingeAudioProcessor::getProgramName (int index)
{
    return {};
}

void TingeAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TingeAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    for (int index = 0; index < 3; index++){
        rotation[index].setSampleRate(sampleRate);
        rotation[index].reset();
    }
    midiProcessor.prepareToPlay(sampleRate);
    
}

void TingeAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TingeAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else

    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void TingeAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    
    midiProcessor.holdPitches(midiMessages);

    for(int sample = 0; sample < buffer.getNumSamples(); ++sample){
        heldPitches = midiProcessor.getheldPitches();
        int numHeldPitches = midiProcessor.getNumHeldNotes();
        
        
        float globalNudgeForward = apvts.getRawParameterValue("globalNudgeForward")->load();
        float globalNudgeBackward = apvts.getRawParameterValue("globalNudgeBackward")->load();
        float globalBrake = apvts.getRawParameterValue("globalBrake")->load();

        for(int index = 0; index < 3; index++)
        {
            juce::String stateID = "state" + juce::String(index);
            juce::String rateFreeID = "rateFree" + juce::String(index);
            juce::String rateSyncID = "rateSync" + juce::String(index);
            juce::String divisionID = "division" + juce::String(index);
            juce::String opacityID = "opacity" + juce::String(index);

            juce::String nudgeForwardID = "nudgeForward" + juce::String(index);
            juce::String nudgeBackwardID = "nudgeBackward" + juce::String(index);
            juce::String nudgeStrengthID = "nudgeSrength" + juce::String(index);

            float state = apvts.getRawParameterValue(stateID)->load();
            float rateFree = apvts.getRawParameterValue(rateFreeID)->load();
            float rateSync = apvts.getRawParameterValue(rateSyncID)->load();
            float division = apvts.getRawParameterValue(divisionID)->load();
            float opacity = apvts.getRawParameterValue(opacityID)->load();

            float nudgeStrength = apvts.getRawParameterValue(nudgeStrengthID)->load();
            float nudgeForward = apvts.getRawParameterValue(nudgeForwardID)->load();
            float nudgeBackward = apvts.getRawParameterValue(nudgeBackwardID)->load();
            
            rotation[index].setRate(index, rateFree, 0);
            rotation[index].nudge(nudgeStrength, nudgeForward, nudgeBackward);
            rotation[index].accumulate();
            phases[index] = rotation[index].getPhase();

            midiProcessor.setSpinnerValues(index, state, rotation[index].getPhase(), division, opacity);

        }
        
        
        // midi output processing
        midiProcessor.setNumThresholds(heldPitches);
        midiProcessor.processThreshold();
        midiProcessor.processAngles();
        midiProcessor.processInteraction();
        
        midiProcessor.notePlayback(midiMessages, sample);
        
        
        phasesAtomic.store(phases);
        heldPitchesAtomic.store(heldPitches);
    }
}

//==============================================================================
bool TingeAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TingeAudioProcessor::createEditor()
{
    return new TingeAudioProcessorEditor (*this, phasesAtomic, heldPitchesAtomic);
}

//==============================================================================
void TingeAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void TingeAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TingeAudioProcessor();
}


juce::AudioProcessorValueTreeState::ParameterLayout
TingeAudioProcessor::createParameterLayout()
{
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID { "globalNudgeForward", 1},
                                                             "Global Nudge Forward",
                                                             0, 1, 0));
    
        layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID { "globalNudgeBackward", 1},
                                                         "Global Nudge Backward",
                                                         0, 1, 0));
    
        layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID { "globalBrake", 1},
                                                     "Global Brake",
                                                     0, 1, 0));


        for(int rotation = 0; rotation < 3; rotation++)
        {
            juce::String stateID = "state" + juce::String(rotation);
            juce::String stateName = "State " + juce::String(rotation);
            
            layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID { stateID, 1},
                                                                   stateName,
                                                                   true));

            juce::String rateFreeID = "rateFree" + juce::String(rotation);
            juce::String rateFreeName = "Rate Free " + juce::String(rotation);
            
            layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { rateFreeID, 1},
                                                                   rateFreeName,
                                                                   juce::NormalisableRange<float> { -5.0f, 5.0f, 0.01 }, 0.0f));
            
            juce::String opacityID = "opacity" + juce::String(rotation);
            juce::String opacityName = "Opacity " + juce::String(rotation);
            
            layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { opacityID, 1},
                                                                   rateFreeName,
                                                                   juce::NormalisableRange<float> { 0.0, 1.0f, 0.01 }, 0.0f));

            juce::String rateSyncID = "rateSync" + juce::String(rotation);
            juce::String rateSyncName = "Rate Sync " + juce::String(rotation);

            layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { rateSyncID, 1},
                                                                   rateSyncName,
                                                                   juce::NormalisableRange<float> { -5.0f, 5.0f, 0.01 }, 0.0f));
            
            juce::String divisionID = "division" + juce::String(rotation);
            juce::String DivisionName = "Division " + juce::String(rotation);

            layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID { divisionID, 1},
                                                                   DivisionName,
                                                                   1, 6, 3));


            juce::String nudgeForwardID = "nudgeForward" + juce::String(rotation);
            juce::String nudgeForwardName = "Nudge Forward" + juce::String(rotation);

            layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID { nudgeForwardID, 1},
                                                                 nudgeForwardName,
                                                                 0, 1, 0));
            
            juce::String nudgeBackwardID = "nudgeBackward" + juce::String(rotation);
            juce::String nudgeBackwardName = "Nudge Backward" + juce::String(rotation);

            layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID { nudgeBackwardID, 1},
                                                                 nudgeBackwardName,
                                                                 0, 1, 0));

            juce::String nudgeStrengthID = "nudgeSrength" + juce::String(rotation);
            juce::String nudgeStrengthName = "Nudge Strength" + juce::String(rotation);

            layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { nudgeStrengthID, 1},
                                                                   nudgeStrengthName,
                                                                   juce::NormalisableRange<float> { 0.0f, 20000.0f, 0.01 }, 4000.0f));
        }

        return layout;
}
