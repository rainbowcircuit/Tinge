/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "UserInterfaceLayout.h"

//==============================================================================
TingeAudioProcessorEditor::TingeAudioProcessorEditor (TingeAudioProcessor& p, std::atomic<std::array<float, 3>>& phases, std::atomic<std::array<float, 16>>& noteValues)
    : AudioProcessorEditor (&p), phasesAtomic(phases), noteValuesAtomic(noteValues), audioProcessor (p)
{
    addAndMakeVisible(spinnerGraphics);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addAndMakeVisible(rate1Slider);
    addAndMakeVisible(rate2Slider);
    addAndMakeVisible(rate3Slider);
    addAndMakeVisible(division1Slider);
    addAndMakeVisible(division2Slider);
    addAndMakeVisible(division3Slider);
    
    rotationLayout1 = std::make_unique<RotationLayout>(audioProcessor, 0);
    rotationLayout2 = std::make_unique<RotationLayout>(audioProcessor, 1);
    rotationLayout3 = std::make_unique<RotationLayout>(audioProcessor, 2);
    
    addAndMakeVisible(*rotationLayout1);
    addAndMakeVisible(*rotationLayout2);
    addAndMakeVisible(*rotationLayout3);

    setSize (400, 600);
    startTimer(30);
}

TingeAudioProcessorEditor::~TingeAudioProcessorEditor()
{
}

//==============================================================================
void TingeAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colour(45, 44, 43));

    
    
}

void TingeAudioProcessorEditor::resized()
{
    rotationLayout1->setBounds(0, 300, 400, 100);
    rotationLayout2->setBounds(0, 400, 400, 100);
    rotationLayout3->setBounds(0, 500, 400, 100);

    spinnerGraphics.setBounds(0, 20, 400, 300);
}

void TingeAudioProcessorEditor::timerCallback()
{
    
    phases = phasesAtomic.load();
    
    
    for (int index = 0; index < 3; index++)
    {
        juce::String stateID = "state" + juce::String(index);
        juce::String divisionID = "division" + juce::String(index);
        juce::String opacityID = "opacity" + juce::String(index);

        int state = audioProcessor.apvts.getRawParameterValue(stateID)->load();
        int division = audioProcessor.apvts.getRawParameterValue(divisionID)->load();
        int opacity = audioProcessor.apvts.getRawParameterValue(opacityID)->load();
    
    spinnerGraphics.setParams(index, phases[index], division, state, opacity);
    }
    
    std::array<float, 16> heldNotes = noteValuesAtomic.load();
    spinnerGraphics.setNumThresholds(heldNotes);
    spinnerGraphics.processThreshold();
    spinnerGraphics.processAngles();
    
}
