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
    
    // global controls layout
    globalLayout = std::make_unique<GlobalControlsLayout>(audioProcessor);
    addAndMakeVisible(*globalLayout);

    // rotation layout
    rotationLayout1 = std::make_unique<SpinnerLayout>(audioProcessor, 0);
    addAndMakeVisible(*rotationLayout1);

    rotationLayout2 = std::make_unique<SpinnerLayout>(audioProcessor, 1);
    addAndMakeVisible(*rotationLayout2);

    rotationLayout3 = std::make_unique<SpinnerLayout>(audioProcessor, 2);
    addAndMakeVisible(*rotationLayout3);

    // threshold layout
    thresholdLayout = std::make_unique<ThresholdLayout>(audioProcessor);
    addAndMakeVisible(*thresholdLayout);

    // tab buttons
    setButton(*this, spinnerTabButton, spinnerTabLAF);
    spinnerTabButton.addListener(this);
    setButton(*this, thresholdTabButton, thresholdTabLAF);
    thresholdTabButton.addListener(this);

    // overlap
    setSlider(*this, overlapSlider, overlapLAF);
    overlapAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params->apvts, "overlap", overlapSlider);
    
    // editor size and animation setup
    setSize (550, 550);
    startTimerHz(60);
    animationSlew.setSampleRate(60);

    
    // initialize parameters
    controlState = audioProcessor.params->apvts.state.getProperty("controlState", 0);
    
    viewState = audioProcessor.params->apvts.state.getProperty("viewState", 0);
    rotationLayout1->setVisible(controlState);
    rotationLayout2->setVisible(controlState);
    rotationLayout3->setVisible(controlState);
    thresholdLayout->setVisible(!controlState);

}

TingeAudioProcessorEditor::~TingeAudioProcessorEditor()
{
    spinnerTabButton.removeListener(this);
    controlWindowToggle.removeListener(this);
    thresholdTabButton.removeListener(this);
}

//==============================================================================
void TingeAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.setColour(Colors::backgroundFill);
    g.fillAll();
    
    auto bounds = getLocalBounds().toFloat();
    float x = bounds.getX();
    float y = bounds.getY();
    float width = bounds.getWidth();
    float height = bounds.getHeight();
    float margin = width * 0.02f;
    bounds.reduce(margin, margin);

    float controlWidth = width * 0.4f;
    float controlXPosition = x + (animationValue * controlWidth * 1.25f) + (width * 0.6f) - (margin/2);

    // backgrounds
    juce::Path bgFillPath, bgAltFillPath;
    juce::Rectangle<float> bgFillBounds = { controlXPosition,
        y + margin * 0.5f,
        controlWidth,
        height - margin };
    
    bgAltFillPath.addRectangle(bgFillBounds);
    bgAltFillPath = bgAltFillPath.createPathWithRoundedCorners(margin);
    g.setColour(Colors::graphicBlackAlt);
    g.fillPath(bgAltFillPath);

    float tabHeight = height * 0.0625f;
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
    
    
    // overlap control
    overlapSlider.setBounds(x + margin,
                            y + margin,
                            height * 0.125f,
                            height * 0.125f);
    overlapSlider.setMouseDragSensitivity(height * 0.25f);

    // rotation
    rotationLayout1->setBounds(controlXPosition,
                               y + margin * 0.5f,
                               controlWidth,
                               height * 0.295f);

    rotationLayout2->setBounds(controlXPosition,
                               y + margin + height * 0.3f,
                               controlWidth,
                               height * 0.295f);
    
    rotationLayout3->setBounds(controlXPosition,
                               y + margin + height * 0.6f,
                               controlWidth,
                               height * 0.295f);
    
    thresholdLayout->setBounds(controlXPosition,
                               y + margin,
                               controlWidth,
                               height - tabHeight);
        
    // spinner
    spinnerGraphics.setAnimation(animationValue);
    spinnerGraphics.setBounds(0,
                              0,
                              (int)(animationValue * width * 0.4f) + (width * 0.6f),
                              height * 0.875f);
        
    globalLayout->setBounds(0,
                            height * 0.875f,
                            (int)(animationValue * width * 0.4f) + (width * 0.6f),
                            height * 0.1f);
    
    // control toggles
    spinnerTabButton.setBounds(controlXPosition,
                                   (y + height - tabHeight) - (margin/2),
                                   width * 0.2f,
                                   tabHeight);
    
    thresholdTabButton.setBounds(controlXPosition + width * 0.2f,
                                   (y + height - tabHeight) - (margin/2),
                                   width * 0.2f,
                                   tabHeight);
     
}

void TingeAudioProcessorEditor::resized()
{

    rotationLayout1->setFontSize(12);
    rotationLayout2->setFontSize(12);
    rotationLayout3->setFontSize(12);
}

void TingeAudioProcessorEditor::timerCallback()
{
    animationSlew.setEnvelopeSlew(3000.0f, 3000.0f);
    animationValue = animationSlew.generateEnvelope();
    repaint();
    
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
    

     
    overlapSlider.repaint();
    
    if (auto* processor = dynamic_cast<TingeAudioProcessor*>(&audioProcessor))
    {
        processor->saveEditorState(viewState, controlState);
        
    }

}

