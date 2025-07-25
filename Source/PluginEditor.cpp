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

    updater.addAnimator(isoEnterToggle);
    updater.addAnimator(isoExitToggle);
    
    addAndMakeVisible(overlapSlider);
    overlapSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    overlapSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 40, 20);
    overlapSlider.setLookAndFeel(&overlapLAF);
    overlapAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "overlap", overlapSlider);
    

    addAndMakeVisible(hueSlider);
    hueSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    hueSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 40, 20);
    hueSlider.setRange(-1.0, 2.0f);

    addAndMakeVisible(offsetSlider);
    offsetSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    offsetSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 40, 20);
    offsetSlider.setRange(0.0, 100.0);

    initializeParameter();
    const auto params = audioProcessor.getParameters();
    for (auto param : params){
        param->addListener(this);
    }
    
    setSize (650, 450);
    startTimerHz(60);
}

TingeAudioProcessorEditor::~TingeAudioProcessorEditor()
{
    const auto params = audioProcessor.
    getParameters();
    for (auto param : params){
        param->removeListener(this);
    }
}

//==============================================================================
void TingeAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colour(180, 180, 180));

    auto bounds = getLocalBounds().toFloat();
    float x = bounds.getX();
    float y = bounds.getY();
    float width = bounds.getWidth();
    float height = bounds.getHeight();

    juce::Path controlFill;
    float xCoords = animationValue * 225 + 225;
    controlFill.addRectangle(xCoords, 0, 225, 450);
    g.setColour(juce::Colour(200, 200, 200));
    
    float controlWidth = width * 0.275f;
    float controlMargin = width * 0.025f;
    rotationLayout1->setBounds(x + (animationValue * controlWidth * 1.25f) + (width * 0.725f) - controlMargin,
                               0,
                               controlWidth,
                               height * 0.35f);
    
    rotationLayout2->setBounds(x - (animationValue * controlWidth * 1.25f) + controlMargin,
                               height * 0.25f,
                               controlWidth,
                               height * 0.35f);
    
    rotationLayout3->setBounds(x + (animationValue * controlWidth * 1.25f) + (width * 0.725f) - controlMargin,
                               height * 0.5f,
                               controlWidth,
                               height * 0.35f);
    
    spinnerGraphics.setAnimation(animationValue);
    spinnerGraphics.setBounds((width - ((animationValue * height * 0.35f) + (height * 0.65f))) * 0.5f,
                              0,
                              (animationValue * height * 0.35f) + (height * 0.65f),
                              height);

    
  //  valueAssignmentLayout->setBounds(xCoords, height * 0.8f, 225, height * 0.25f);
    
    globalLayout->setBounds(0, 400, width, 50);
    
}

void TingeAudioProcessorEditor::resized()
{
    overlapSlider.setBounds(5, 5, 80, 80);
    
}


void TingeAudioProcessorEditor::timerCallback()
{
    phases = phasesAtomic.load();

    for (int index = 0; index < 3; index++)
    {
        spinnerGraphics.setPhase(index, phases[index]);
    }
    
    std::array<float, 16> heldNotes = noteValuesAtomic.load();
    spinnerGraphics.setNumThresholds(heldNotes);
    spinnerGraphics.processThreshold();
    spinnerGraphics.processAngles();
}

void TingeAudioProcessorEditor::parameterValueChanged (int parameterIndex, float newValue)
{
    juce::String newParameterID;
    float scaledValue = 0.0f;

    if (auto* param = dynamic_cast<juce::AudioProcessorParameterWithID*>(audioProcessor.getParameters()[parameterIndex]))
    {
        if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(param))
        {
            scaledValue = rangedParam->convertFrom0to1(newValue);
            newParameterID = param->paramID;
        }
    }

    nudgeStrength = juce::String("nudgeStrength") == newParameterID ? scaledValue : nudgeStrength;

    for (int index = 0 ; index < 3; index++){
        auto incr = juce::String(index);
        
        ratio[index] = juce::String("ratio") + incr == newParameterID ? scaledValue : ratio[index];
        state[index] = juce::String("state") + incr == newParameterID ? scaledValue : state[index];
        colorIndex[index] = juce::String("colorIndex") + incr == newParameterID ? scaledValue : colorIndex[index];
        opacity[index] = juce::String("opacity") + incr == newParameterID ? scaledValue : opacity[index];

        spinnerGraphics.setParams(index, ratio[index], state[index], colorIndex[index], opacity[index]);
    }
    
    globalLayout->setLPGStrength(nudgeStrength);
    
    overlap = juce::String("overlap") == newParameterID ? scaledValue : overlap;
    spinnerGraphics.setOverlapIndex(overlap);
    overlapLAF.setColor(colorIndex[0], colorIndex[1], colorIndex[2], opacity[0], opacity[1], opacity[2]);
    overlapSlider.repaint();
}

void TingeAudioProcessorEditor::initializeParameter()
{
    nudgeStrength = audioProcessor.apvts.getRawParameterValue(juce::String("nudgeStrength"))->load();
    globalLayout->setLPGStrength(nudgeStrength);

    
    overlap = audioProcessor.apvts.getRawParameterValue(juce::String("overlap"))->load();
    spinnerGraphics.setOverlapIndex(overlap);
    overlapLAF.setColor(colorIndex[0], colorIndex[1], colorIndex[2], opacity[0], opacity[1], opacity[2]);
    overlapSlider.repaint();

    for (int index = 0 ; index < 3; index++){
        auto incr = juce::String(index);
        
        ratio[index] = audioProcessor.apvts.getRawParameterValue(juce::String("ratio") + incr)->load();
        state[index] = audioProcessor.apvts.getRawParameterValue(juce::String("state") + incr)->load();
        opacity[index] = audioProcessor.apvts.getRawParameterValue(juce::String("opacity") + incr)->load();
        colorIndex[index] = audioProcessor.apvts.getRawParameterValue(juce::String("colorIndex") + incr)->load();

        spinnerGraphics.setParams(index, ratio[index], state[index], colorIndex[index], opacity[index]);
        
    }

}

