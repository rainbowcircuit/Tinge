/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "UserInterfaceLayout.h"

//==============================================================================
TingeAudioProcessorEditor::TingeAudioProcessorEditor (TingeAudioProcessor& p, std::atomic<float>& phase1, std::atomic<float>& phase2, std::atomic<float>& phase3)
    : AudioProcessorEditor (&p), phaseAtomic1(phase1), phaseAtomic2(phase2), phaseAtomic3(phase3), audioProcessor (p)
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
    g.fillAll(juce::Colour(200, 200, 200));

    
    
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
    phase0 = phaseAtomic1.load();
    phase1 = phaseAtomic2.load();
    phase2 = phaseAtomic3.load();

    int division0 = audioProcessor.apvts.getRawParameterValue("division0")->load();
    int division1 = audioProcessor.apvts.getRawParameterValue("division1")->load();
    int division2 = audioProcessor.apvts.getRawParameterValue("division2")->load();

    int state0 = audioProcessor.apvts.getRawParameterValue("state0")->load();
    int state1 = audioProcessor.apvts.getRawParameterValue("state1")->load();
    int state2 = audioProcessor.apvts.getRawParameterValue("state2")->load();

    float opacity0 = audioProcessor.apvts.getRawParameterValue("opacity0")->load();
    float opacity1 = audioProcessor.apvts.getRawParameterValue("opacity1")->load();
    float opacity2 = audioProcessor.apvts.getRawParameterValue("opacity2")->load();

    spinnerGraphics.setParams(0, phase0, division0, state0, opacity0);
    spinnerGraphics.setParams(1, phase1, division1, state1, opacity1);
    spinnerGraphics.setParams(2, phase2, division2, state2, opacity2);

    
}
