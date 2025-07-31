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
    initializeParameters();
    auto params = this->getParameters();
    for (auto param : params)
    {
        param->addListener(this);
    }
}

TingeAudioProcessor::~TingeAudioProcessor()
{
    auto params = this->getParameters();
    for (auto param : params)
    {
        param->removeListener(this);
    }
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
    const auto params = this->getParameters();
    for (auto param : params){
        param->addListener(this);
    }
    
    initializeParameters();
}

void TingeAudioProcessor::releaseResources()
{
    const auto params = this->getParameters();
    for (auto param : params){
        param->removeListener(this);
    }
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
    heldPitches = midiProcessor.getheldPitches();

    midiProcessor.setOverlap(overlap);

    for (int i = 0; i < 3; i++){
        rotation[i].tempo(getPlayHead());
        rotation[i].setRate(rateSync[i], rateFree[i], rateMode[i], phase[i]);
        rotation[i].nudge(nudgeStrength, nudgeForward, nudgeBackward, brake);

        for(int sample = 0; sample < buffer.getNumSamples(); ++sample){
            rotation[i].accumulate();
        }
        
        midiProcessor.setSpinnerValues(i,
                                       state[i],
                                       rotation[i].getPhase(), // real time rotation
                                       ratio[i],
                                       opacity[i]);
        
        phases[i] = rotation[i].getPhase(); // for atomic
    }
    midiProcessor.processInteraction();
    midiProcessor.notePlayback(midiMessages);

    phasesAtomic.store(phases);
    heldPitchesAtomic.store(heldPitches);

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

    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID { "nudgeForward", 1},
                                                         "Nudge Forward",
                                                         0, 1, 0));

    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID { "nudgeBackward", 1},
                                                     "Nudge Backward",
                                                     0, 1, 0));

    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID { "brake", 1},
                                                 "Brake",
                                                 0, 1, 0));

    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "nudgeStrength", 1},
                                                           "Nudge Strength",
                                                           juce::NormalisableRange<float> { 0.0f, 100.0f, 0.1 }, 100.0f));

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
    
    
    
    std::array<float, 3> phaseDefaults = { 0.0f, 33.0f, 66.0f };
    std::array<int, 3> colorIndexDefaults = { 0, 7, 15 };
    std::array<bool, 3> stateDefaults = { false, false, true };

    for(int rotation = 0; rotation < 3; rotation++)
    {
        juce::String stateID = "state" + juce::String(rotation);
        juce::String stateName = "State " + juce::String(rotation);
        
        layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID { stateID, 1},
                                                               stateName,
                                                              stateDefaults[rotation]));

        juce::String rateFreeID = "rateFree" + juce::String(rotation);
        juce::String rateFreeName = "Rate Free " + juce::String(rotation);
        
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { rateFreeID, 1},
                                                               rateFreeName,
                                                               juce::NormalisableRange<float> { -5.0f, 5.0f, 0.01 }, 0.0f));
        
        juce::String rateSyncID = "rateSync" + juce::String(rotation);
        juce::String rateSyncName = "Rate Sync " + juce::String(rotation);

        layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID { rateSyncID, 1},
                                                                rateSyncName, juce::StringArray {
            "-1/4",      // 16th
            "-1/3",      // 8th triplet
            "-3/8",      // 16th dotted
            "-1/2",      // 8th
            "-2/3",      // quarter triplet
            "-3/4",      // 8th dotted
            "-1",        // quarter
            "-4/3",      // half triplet
            "-3/2",      // quarter dotted
            "-2",        // half
            "-8/3",      // whole triplet
            "-3",        // half dotted
            "-4",        // whole
            "-16/3",     // 2 bar triplet
            "-6",        // whole dotted
            "-8",        // 2 bars
            "-12",       // 2 bar dotted
            "-16",       // 4 bars
            "-32",       // 8 bars
            "0",        // static
            "32",      // Reverse 8 bars
            "16",      // Reverse 4 bars
            "12",      // Reverse 2 bar dotted
            "8",       // Reverse 2 bars
            "6",       // Reverse whole dotted
            "16/3",    // Reverse 2 bar triplet
            "4",       // Reverse whole
            "3",       // Reverse half dotted
            "8/3",     // Reverse whole triplet
            "2",       // Reverse half
            "3/2",     // Reverse quarter dotted
            "4/3",     // Reverse half triplet
            "1",       // Reverse quarter
            "3/4",     // Reverse 8th dotted
            "2/3",     // Reverse quarter triplet
            "1/2",     // Reverse 8th
            "3/8",     // Reverse 16th dotted
            "1/3",     // Reverse 8th triplet
            "1/4",     // Reverse 16th
        }, 19));
        
        juce::String rateModeID = "rateMode" + juce::String(rotation);
        juce::String rateModeName = "Rate Mode " + juce::String(rotation);
        
        layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID { rateModeID, 1},
                                                              rateModeName,
                                                               true));

        
        juce::String ratioID = "ratio" + juce::String(rotation);
        juce::String ratioName = "Ratio " + juce::String(rotation);

        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { ratioID, 1},
                                                               ratioName,
                                                               juce::NormalisableRange<float> { 1.0f, 5.0f, 0.01 }, 0.0f));

        juce::String phaseID = "phase" + juce::String(rotation);
        juce::String phaseName = "Phase " + juce::String(rotation);

        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { phaseID, 1},
                                                               phaseName,
                                                               juce::NormalisableRange<float> { 0.0f, 100.0f, 0.01 }, phaseDefaults[rotation]));

        juce::String opacityID = "opacity" + juce::String(rotation);
        juce::String opacityName = "Opacity " + juce::String(rotation);
        
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { opacityID, 1},
                                                               rateFreeName,
                                                               juce::NormalisableRange<float> { 0.0, 1.00f, 0.01 }, 100.0f));

        juce::String colorIndexID = "colorIndex" + juce::String(rotation);
        juce::String colorIndexName = "Color Choice " + juce::String(rotation);

        layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID { colorIndexID, 1},
                                                                colorIndexName, juce::StringArray {
            "Yellow", "Chartreuse", "Yellow-Green", "Spring-Green", "Green", "Blue-Green",
            "Aqua-Green", "Aqua-Blue", "Turquoise-Blue", "Cerulean-Blue", "Blue", "Blue-Violet",
            "Violet", "Red-Violet", "Purple", "Fuchsia", "Magenta", "Blue-Red", "Red",
            "Orange-Red", "Orange", "Yellow-Orange", "Orange-Yellow", "Golden-Yellow"
        }, colorIndexDefaults[rotation]));
    } 

    return layout;
}
