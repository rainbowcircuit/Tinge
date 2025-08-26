/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TingeAudioProcessorEditor::TingeAudioProcessorEditor (TingeAudioProcessor& p, std::atomic<std::array<float, 3>>& phases, std::atomic<std::array<float, 16>>& noteValues)
    : AudioProcessorEditor (&p), phasesAtomic(phases), noteValuesAtomic(noteValues), audioProcessor (p)
{
    addAndMakeVisible(spinnerGraphics);
    spinnerGraphics.setInterceptsMouseClicks(false, false);
    
    presetLayout = std::make_unique<PresetControlsLayout>(audioProcessor, *this, audioProcessor.params->apvts);

    rotationLayout1 = std::make_unique<SpinnerLayout>(audioProcessor, 0);
    rotationLayout2 = std::make_unique<SpinnerLayout>(audioProcessor, 1);
    rotationLayout3 = std::make_unique<SpinnerLayout>(audioProcessor, 2);
    
    globalLayout = std::make_unique<GlobalControlsLayout>(audioProcessor);
    
    addAndMakeVisible(*presetLayout);
    addAndMakeVisible(*rotationLayout1);
    addAndMakeVisible(*rotationLayout2);
    addAndMakeVisible(*rotationLayout3);
    addAndMakeVisible(*globalLayout);

    
    addAndMakeVisible(overlapSlider);
    overlapSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    overlapSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 40, 20);
   // overlapSlider.setLookAndFeel(&overlapLAF);
    overlapAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params->apvts, "overlap", overlapSlider);
    

    setSize (550, 550);
    startTimerHz(60);
    
    updater.addAnimator(isoEnterToggle);
    updater.addAnimator(isoExitToggle);
 
    setButton(*this, controlWindowToggle, laf);
    controlWindowToggle.addListener(this);
    
}

TingeAudioProcessorEditor::~TingeAudioProcessorEditor()
{
    controlWindowToggle.removeListener(this);
}

//==============================================================================
void TingeAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colour(210, 210, 210));

    auto bounds = getLocalBounds().toFloat();
    bounds.reduce(getMargin(), getMargin());
    float x = bounds.getX();
    float y = bounds.getY();
    float width = bounds.getWidth();
    float height = bounds.getHeight();

    juce::Path bgFillPath, bgTabPath;
    float xCoords = (animationValue * width * 0.4f) + width * 0.6f;
    float margin = width * 0.02f;
    bgFillPath.addRoundedRectangle(xCoords,
                                   y + margin * 0.5f,
                                   width * 0.4f,
                                   height - margin,
                                   margin);
    
    g.setColour(juce::Colour(203, 203, 203));
    g.fillPath(bgFillPath);
    g.setColour(juce::Colour(200, 200, 200));
    g.strokePath(bgFillPath, juce::PathStrokeType(1.5f));

    bgTabPath.addRoundedRectangle(xCoords - width * 0.04f,
                                  margin + height * 0.2f,
                                  width * 0.04f,
                                  height * 0.08f,
                                  margin/2,
                                  margin/2,
                                  true,
                                  false,
                                  true,
                                  false);
    
    g.setColour(juce::Colour(203, 203, 203));
    g.fillPath(bgTabPath);
    g.setColour(juce::Colour(200, 200, 200));
    g.strokePath(bgTabPath, juce::PathStrokeType(1.5f));

    
    
    
    float controlWidth = width * 0.4f;
    float controlMargin = width * 0.025f;
    
    overlapSlider.setBounds(x + margin,
                            y + margin,
                            height * 0.175f,
                            height * 0.175f);
    overlapSlider.setMouseDragSensitivity(height * 0.25f);
    
    /*
    presetLayout->setBounds(x + (animationValue * controlWidth) + (width * 0.6f) - controlMargin * 0.5f + (margin/2),
                             y + margin,
                             controlWidth - margin/2,
                             height * 0.15f);
     */
    rotationLayout1->setBounds(x + (animationValue * controlWidth) + (width * 0.6f) - (margin * 0.5f),
                               y + margin * 0.5f,
                               controlWidth,
                               height * 0.25f);


    rotationLayout2->setBounds(x + (animationValue * controlWidth) + (width * 0.6f) - (margin * 0.5f),
                               y + margin + height * 0.3f,
                               controlWidth,
                               height * 0.25f);
    
    rotationLayout3->setBounds(x + (animationValue * controlWidth) + (width * 0.6f) - (margin * 0.5f),
                               y + margin + height * 0.6f,
                               controlWidth,
                               height * 0.25f);
    
    spinnerGraphics.setAnimation(animationValue);
    spinnerGraphics.setBounds(0,
                              0,
                              (int)(animationValue * width * 0.4f) + (width * 0.6f),
                              height);
        
    globalLayout->setBounds(0,
                            height * 0.875f,
                            (int)(animationValue * width * 0.4f) + (width * 0.6f),
                            height * 0.1f);
    /*
    controlWindowToggle.setBounds(x + (animationValue * controlWidth) + (width * 0.6f) - controlMargin * 0.5f,
    y,
    width * 0.025f,
    height * 0.05f);
    */
}

void TingeAudioProcessorEditor::resized()
{
    rotationLayout1->setFontSize(12);
    rotationLayout2->setFontSize(12);
    rotationLayout3->setFontSize(12);
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
    spinnerGraphics.processThreshold(static_cast<MIDIProcessor::thresholdMode>(audioProcessor.params->thresholdMode->getSafe()));
    spinnerGraphics.processAngles();
    
    
    for (int index = 0 ; index < 3; index++){

        spinnerGraphics.setParams(index,
                                  1,
                                  1,
                                  audioProcessor.params->weight[index]->getSafe());
    }
    
    
    spinnerGraphics.setOverlapIndex(audioProcessor.params->overlap->getSafe());
    /*
    overlapLAF.setColor(1, 1, 1,
                        audioProcessor.params->weight[0]->getSafe(),
                        audioProcessor.params->weight[1]->getSafe(),
                        audioProcessor.params->weight[2]->getSafe());
     */
    overlapSlider.repaint();
}

