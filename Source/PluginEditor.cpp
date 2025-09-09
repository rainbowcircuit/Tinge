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
    
    thresholdLayout = std::make_unique<ThresholdLayout>(audioProcessor);

    globalLayout = std::make_unique<GlobalControlsLayout>(audioProcessor);
    
    addAndMakeVisible(*presetLayout);
    addAndMakeVisible(*rotationLayout1);
    addAndMakeVisible(*rotationLayout2);
    addAndMakeVisible(*rotationLayout3);
    addAndMakeVisible(*thresholdLayout);

    addAndMakeVisible(*globalLayout);

    addAndMakeVisible(spinnerControlToggle);
    spinnerControlToggle.addListener(this);
    addAndMakeVisible(thresholdControlToggle);
    thresholdControlToggle.addListener(this);


    // overlap
    setLabel(*this, overlapLabel, "Overlap", juce::Justification::centred);
    
    setSlider(*this, overlapSlider, overlapLAF);
    overlapAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params->apvts, "overlap", overlapSlider);
    

    setSize (550, 550);
    startTimerHz(60);
    
    updater.addAnimator(isoEnterToggle);
    updater.addAnimator(isoExitToggle);
 
    controlState = audioProcessor.params->apvts.state.getProperty("controlState", 0);
    viewState = audioProcessor.params->apvts.state.getProperty("viewState", 0);
    rotationLayout1->setVisible(!controlState);
    rotationLayout2->setVisible(!controlState);
    rotationLayout3->setVisible(!controlState);
    thresholdLayout->setVisible(controlState);

    
}

TingeAudioProcessorEditor::~TingeAudioProcessorEditor()
{
    spinnerControlToggle.removeListener(this);
    controlWindowToggle.removeListener(this);
    thresholdControlToggle.removeListener(this);
}

//==============================================================================
void TingeAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.setColour(Colors::backgroundFill);
    g.fillAll();
    
    auto bounds = getLocalBounds().toFloat();
    bounds.reduce(getMargin(), getMargin());
    float x = bounds.getX();
    float y = bounds.getY();
    float width = bounds.getWidth();
    float height = bounds.getHeight();

    juce::Path bgFillPath;
    float xCoords = (animationValue * width * 0.4f) + width * 0.6f;
    float margin = width * 0.02f;
    
    juce::Rectangle<float> bgFillBounds = {xCoords,
        y + margin * 0.5f,
        width * 0.4f,
        height - margin};
    
    float tabHeight = height * 0.05f;
    float tabRightHeight = !controlState ? 0.0f : tabHeight;
    float tabLeftHeight = controlState ? 0.0f : tabHeight;

    bgFillPath.startNewSubPath(bgFillBounds.getTopLeft());
    bgFillPath.lineTo(bgFillBounds.getTopRight());
    
    bgFillPath.lineTo(bgFillBounds.getBottomRight().x,
                      bgFillBounds.getBottomRight().y - tabRightHeight);
    bgFillPath.lineTo(bgFillBounds.getCentreX(),
                      bgFillBounds.getY() + bgFillBounds.getHeight() - tabRightHeight);
    
    bgFillPath.lineTo(bgFillBounds.getCentreX(),
                      bgFillBounds.getY() + bgFillBounds.getHeight() - tabHeight);
    
    bgFillPath.lineTo(bgFillBounds.getCentreX(),
                      bgFillBounds.getY() + bgFillBounds.getHeight() - tabLeftHeight);
    
    bgFillPath.lineTo(bgFillBounds.getBottomLeft().x,
                      bgFillBounds.getBottomLeft().y - tabLeftHeight);
    
    bgFillPath.closeSubPath();
    bgFillPath = bgFillPath.createPathWithRoundedCorners(margin);

    g.setColour(Colors::graphicBlack);
    g.fillPath(bgFillPath);
    g.setColour(Colors::backgroundFillAlt);
    g.strokePath(bgFillPath, juce::PathStrokeType(1.5f));
    
    float controlWidth = width * 0.4f;
    
    // overlap control
    overlapSlider.setBounds(x,
                            y,
                            height * 0.125f,
                            height * 0.125f);
    overlapSlider.setMouseDragSensitivity(height * 0.25f);

    rotationLayout1->setBounds(x + (animationValue * controlWidth) + (width * 0.6f) - (margin * 0.5f),
                               y + margin * 0.5f,
                               controlWidth,
                               height * 0.3f);


    rotationLayout2->setBounds(x + (animationValue * controlWidth) + (width * 0.6f) - (margin * 0.5f),
                               y + margin + height * 0.3f,
                               controlWidth,
                               height * 0.3f);
    
    rotationLayout3->setBounds(x + (animationValue * controlWidth) + (width * 0.6f) - (margin * 0.5f),
                               y + margin + height * 0.6f,
                               controlWidth,
                               height * 0.3f);
    
    thresholdLayout->setBounds(x + (animationValue * controlWidth) + (width * 0.6f) - (margin * 0.5f),
                               y + margin,
                               controlWidth,
                               height);

    
    spinnerGraphics.setAnimation(animationValue);
    spinnerGraphics.setBounds(0,
                              0,
                              (int)(animationValue * width * 0.4f) + (width * 0.6f),
                              height * 0.875f);
        
    globalLayout->setBounds(0,
                            height * 0.9f,
                            (int)(animationValue * width * 0.4f) + (width * 0.6f),
                            height * 0.1f);
    
    // control toggles
    spinnerControlToggle.setBounds(xCoords,
                                   (y + height * 0.95f) - (margin/2),
                                   width * 0.2f,
                                   height * 0.05f);
    
    
    thresholdControlToggle.setBounds(xCoords + width * 0.2f,
                                   (y + height * 0.95f) - (margin/2),
                                   width * 0.2f,
                                   height * 0.05f);
     
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
        spinnerGraphics.setParams(index,
                                  phases[index],
                                  audioProcessor.params->opacity[0]->getSafe(),
                                  audioProcessor.params->opacity[1]->getSafe(),
                                  audioProcessor.params->opacity[2]->getSafe());
    }
    
    std::array<float, 16> heldNotes = noteValuesAtomic.load();
    spinnerGraphics.setNumThresholds(heldNotes);
    
    spinnerGraphics.processThreshold(static_cast<MIDIProcessor::thresholdMode>(audioProcessor.params->thresholdMode->getSafe()),
                                     audioProcessor.params->thresholdPhase->getSafe(),
                                     audioProcessor.params->maxThreshold->getSafe());
        
    thresholdLayout->graphics.setParams(audioProcessor.params->thresholdMode->getSafe(),
                                        audioProcessor.params->thresholdPhase->getSafe(),
                                        audioProcessor.params->maxThreshold->getSafe());
    
    spinnerGraphics.setOverlapIndex(audioProcessor.params->overlap->getSafe());
    
    overlapLAF.setColor(audioProcessor.params->opacity[0]->getSafe(),
                        audioProcessor.params->opacity[1]->getSafe(),
                        audioProcessor.params->opacity[2]->getSafe());
     
    overlapSlider.repaint();
    
    if (auto* processor = dynamic_cast<TingeAudioProcessor*>(&audioProcessor))
    {
        processor->saveEditorState(viewState, controlState);
        
    }

}

