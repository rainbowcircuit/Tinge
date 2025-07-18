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
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
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
    return JucePlugin_IsMidiEffect;
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
        rotation[index].reset();
        rotation[index].setSampleRate(sampleRate);
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

    /*
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    */
    
    midiProcessor.holdPitches(midiMessages);
    
    float globalNudgeForward = apvts.getRawParameterValue("globalNudgeForward")->load();
    float globalNudgeBackward = apvts.getRawParameterValue("globalNudgeBackward")->load();
    float globalBrake = apvts.getRawParameterValue("globalBrake")->load();
    float globalStrength = apvts.getRawParameterValue("globalStrength")->load();

    float noteScale = apvts.getRawParameterValue("noteScale")->load();
    
    float velocityScale = apvts.getRawParameterValue("velocityScale")->load();
    float controlScale = apvts.getRawParameterValue("controlScale")->load();

    float overlap = apvts.getRawParameterValue("overlap")->load();
    midiProcessor.setOverlap(overlap);

    for(int sample = 0; sample < buffer.getNumSamples(); ++sample){
        heldPitches = midiProcessor.getheldPitches();
        
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
            
            rotation[index].setRate(index, rateFree, 0, phase);
            rotation[index].nudge(globalStrength,
                                  globalNudgeForward,
                                  globalNudgeBackward,
                                  globalBrake); // eventually only "global nudges
            rotation[index].accumulate();
            phases[index] = rotation[index].getPhase();

            midiProcessor.setScaling(noteScale, velocityScale, controlScale, 0.0f);
            midiProcessor.setSpinnerValues(index, state, rotation[index].getPhase(), division, opacity);
        }
        
        // midi output processing
        midiProcessor.setNumThresholds(heldPitches);
        midiProcessor.processThreshold();
        midiProcessor.processAngles();
        midiProcessor.processInteraction();
        

        
        phasesAtomic.store(phases);
        heldPitchesAtomic.store(heldPitches);

    }
    midiProcessor.notePlayback(midiMessages);

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
}

void TingeAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
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

    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "globalStrength", 1},
                                                           "Global Strength",
                                                           juce::NormalisableRange<float> { 0.0f, 100.0f, 0.1 }, 0.0f));

    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID { "globalReset", 1},
                                                 "Global Reset",
                                                 0, 1, 0));

    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID { "overlap", 1},
                                                           "Overlap", 0, 6, 6));

    
    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID { "noteScale", 1},
                                                           "Note Scale", -36, 36, 0));
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID { "keyQuantize", 1},
                                                            "Scale Choices", juce::StringArray { "C", "C#/Db", "D", "D#/Eb", "E", "F", "F#/Gb", "G", "G#/Ab", "A", "A#/Bb", "B" }, 0));

    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID { "scaleQuantize", 1},
                                                            "Scale Choices", juce::StringArray { "Chromatic", "Major", "Major Pentatonic", "Lydian", "Mixolydian", "Phrygian Dominant", "Natural Minor", "Minor Pentatonic", "Harmonic Minor", "Melodic Minor", "Dorian", "Phrygian", "Locrian" }, 0));
    
    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID { "velocityScale", 1},
                                                           "velocity Scale", -127, 127, 0));

    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID { "controlScale", 1},
                                                           "Control Scale", -127, 127, 0));
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID { "resetMode", 1},
                                                        "Reset Mode",
                                                        juce::StringArray { "No Reset", "External Reset", "Reset on Note Clear", "Reset On Note On" }, 0));
    
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

        layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID { rateSyncID, 1},
                                                                rateSyncName, juce::StringArray {
            "Reverse 32nd Triplet", "Reverse 32nd", "Reverse 16th Triplet", "Reverse 16th",
            "Reverse 8th Triplet", "Reverse 16th Dotted", "Reverse 8th", "Reverse Quarter Triplet",
            "Reverse 8th Dotted", "Reverse Quarter", "Reverse Half Triplet", "Reverse Quarter Dotted",
            "Reverse Half", "Reverse Whole Triplet", "Reverse Half Dotted", "Reverse Whole",
            "Reverse 2 Measures Triplet", "Reverse Whole Dotted", "Reverse 2 Measures", "Reverse 2 Measures Dotted",
            "Reverse 4 Measures", "Reverse 8 Measures", "Static", "8 Measures", "4 Measures",
            "2 Measures Dotted", "2 Measures", "Whole Dotted", "2 Measures Triplet", "Whole",
            "Half Dotted", "Whole Triplet", "Half", "Quarter Dotted",
            "Half Triplet", "Quarter", "8th Dotted", "Quarter Triplet",
            "8th", "16th Dotted", "8th Triplet", "16th", "16th Triplet", "32nd", "32nd Triplet"
        }, 22));

        juce::String divisionID = "division" + juce::String(rotation);
        juce::String DivisionName = "Division " + juce::String(rotation);

        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { divisionID, 1},
                                                               DivisionName,
                                                               juce::NormalisableRange<float> { 1.0f, 5.0f, 0.01 }, 0.0f));

        juce::String phaseID = "phase" + juce::String(rotation);
        juce::String phaseName = "Phase " + juce::String(rotation);

        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { phaseID, 1},
                                                               phaseName,
                                                               juce::NormalisableRange<float> { 0.0f, 100.0f, 0.01 }, 0.0f));

        juce::String nudgeStrengthID = "nudgeSrength" + juce::String(rotation);
        juce::String nudgeStrengthName = "Nudge Strength" + juce::String(rotation);

        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { nudgeStrengthID, 1},
                                                               nudgeStrengthName,
                                                               juce::NormalisableRange<float> { 0.0f, 20000.0f, 0.01 }, 10000.0f));
         
    }

    return layout;
}
