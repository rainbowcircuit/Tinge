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
#if JucePlugin_Build_VST3
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#else
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), false)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), false)
#endif
                       )
#endif
{
    params = std::make_unique<Parameters>(*this);
}

TingeAudioProcessor::~TingeAudioProcessor() {}

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


// MODIFY THIS METHOD - override the format-specific behavior
bool TingeAudioProcessor::isMidiEffect() const
{
#if JucePlugin_Build_VST3
    return false;  // VST3: not a MIDI effect
#else
    return true;   // AU: is a MIDI effect
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
        rotation[index].reset();
        rotation[index].prepareToPlay(sampleRate, samplesPerBlock);
    }
    midiProcessor.prepareToPlay(sampleRate);
}

void TingeAudioProcessor::releaseResources()
{
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
    midiProcessor.setHold(midiMessages, (bool)params->hold->get());
    
    heldPitches = midiProcessor.getheldPitches();
    
    midiProcessor.setOverlap(params->overlap->get());
    midiProcessor.setSlewAmount(params->valueSlew->get());


    for (int i = 0; i < 3; i++){
        rotation[i].playhead(getPlayHead());
        
        rotation[i].setRate(params->rateSync[i]->get(),
                            params->rateFree[i]->get(),
                            params->rateMode[i]->get(),
                            params->phase[i]->get(),
                            params->curve[i]->get());
        
        rotation[i].nudge(params->nudgeStrength->get(),
                          params->nudgeForward->get(),
                          params->nudgeBackward->get(),
                          params->brakeStrength->get(),
                          params->brake->get());
        
        rotation[i].resetMode(params->resetMode->get(),
                              midiProcessor.getNumHeldNotes(),
                              params->reset->get());
        

        for(int sample = 0; sample < buffer.getNumSamples(); ++sample){
            rotation[i].accumulate();
        }
        
        midiProcessor.setSpinnerValues(i,
                                       rotation[i].getPhase(), // real time rotation
                                       params->opacity[i]->get());
        
        phases[i] = rotation[i].getPhase(); // for atomic
    }
    midiProcessor.processInteraction(params->thresholdMode->get(),
                                     params->thresholdPhase->get(),
                                     params->maxThreshold->get());
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
    copyXmlToBinary(*params->apvts.copyState().createXml(), destData);
}

void TingeAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    const auto xmlState = getXmlFromBinary(data, sizeInBytes);
       if (xmlState == nullptr)
           return;
       const auto newTree = juce::ValueTree::fromXml(*xmlState);
    params->apvts.replaceState(newTree);
}

void TingeAudioProcessor::saveEditorState(bool viewState, bool controlState)
{
    params->apvts.state.setProperty("viewState", viewState, nullptr);
    params->apvts.state.setProperty("controlState", controlState, nullptr);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TingeAudioProcessor();
}
