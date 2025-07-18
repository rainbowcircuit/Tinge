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
    spinnerGraphics.setInterceptsMouseClicks(false, false);

    
    rotationLayout1 = std::make_unique<RotationLayout>(audioProcessor, 0);
    rotationLayout2 = std::make_unique<RotationLayout>(audioProcessor, 1);
    rotationLayout3 = std::make_unique<RotationLayout>(audioProcessor, 2);
    valueAssignmentLayout = std::make_unique<ValueAssignmentLayout>(audioProcessor);
    
    globalLayout = std::make_unique<GlobalControlLayout>(audioProcessor);
    
    addAndMakeVisible(*rotationLayout1);
    addAndMakeVisible(*rotationLayout2);
    addAndMakeVisible(*rotationLayout3);
    addAndMakeVisible(*globalLayout);
    addAndMakeVisible(*valueAssignmentLayout);

    
    setSize (450, 450);
    startTimer(30);
    
    updater.addAnimator(isoEnterToggle);
    updater.addAnimator(isoExitToggle);
    
    
    
    
    addAndMakeVisible(noteScaleSlider);
    noteScaleSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    noteScaleSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
    noteScaleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "noteScale", noteScaleSlider);

    
    addAndMakeVisible(velocityScaleSlider);
    velocityScaleSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    velocityScaleSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
    velocityScaleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "velocityScale", velocityScaleSlider);


    addAndMakeVisible(controlScaleSlider);
    controlScaleSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    controlScaleSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
    controlScaleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "controlScale", controlScaleSlider);
    
    addAndMakeVisible(overlapSlider);
    overlapSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    overlapSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 40, 20);
    overlapSlider.setLookAndFeel(&overlapLAF);
    controlScaleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "overlap", overlapSlider);


}

TingeAudioProcessorEditor::~TingeAudioProcessorEditor()
{
}

//==============================================================================
void TingeAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(Colors::backgroundFill);

    auto bounds = getLocalBounds().toFloat();
    float x = bounds.getX();
    float y = bounds.getY();
    float width = bounds.getWidth();
    float height = bounds.getHeight();

    juce::Path controlFill;
    float xCoords = animationValue * 225 + 225;
    controlFill.addRectangle(xCoords, 0, 225, 450);
    g.setColour(Colors::backgroundFillAlt);
    g.fillPath(controlFill);
    
    rotationLayout1->setBounds(xCoords, 0, width * 0.5f, height * 0.25f);
    rotationLayout2->setBounds(xCoords, height * 0.275f, width * 0.5f, height * 0.25f);
    rotationLayout3->setBounds(xCoords, height * 0.55f, width * 0.5f, height * 0.25f);
    
    valueAssignmentLayout->setBounds(xCoords, height * 0.8f, 225, height * 0.25f);
    
//    globalLayout->setBounds(0, 400, xCoords, 40);
    spinnerGraphics.setAnimation(animationValue);
    spinnerGraphics.setBounds(0, 0, xCoords, 450);
    
    
    //noteScaleSlider.setBounds(10, 10, 80, 60);
    //velocityScaleSlider.setBounds(80, 10, 80, 60);
    //controlScaleSlider.setBounds(10, 80, 80, 60);
    
    
    overlapSlider.setBounds(5, 5, 80, 80);

}

void TingeAudioProcessorEditor::resized()
{
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
    
    int overlap = audioProcessor.apvts.getRawParameterValue("overlap")->load();

    spinnerGraphics.setOverlapIndex(overlap);

    
    std::array<float, 16> heldNotes = noteValuesAtomic.load();
    spinnerGraphics.setNumThresholds(heldNotes);
    spinnerGraphics.processThreshold();
    spinnerGraphics.processAngles();
    
}
