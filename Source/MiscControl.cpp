/*
  ==============================================================================

    MiscControl.cpp
    Created: 20 Aug 2025 11:13:13am
    Author:  Takuma Matsui

  ==============================================================================
*/

#include "MiscControl.h"

void GlobalControlsLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    bounds.reduce(5, 5);
    float x = bounds.getX();
    float y = bounds.getY();
    float width = bounds.getWidth();

    float state = button.getToggleState();
    switch(lookAndFeel)
    {
        case GlobalControlsLAF::NudgeForward:
        {
            drawNudge(g, x, y, width, 3.14f);
            break;
        }
        case GlobalControlsLAF::NudgeBackward:
        {
            drawNudge(g, x, y, width, 0.0f);
            break;
        }
        case GlobalControlsLAF::Brake:
        {
            drawBrake(g, x, y, width);
            break;
        }
        case GlobalControlsLAF::Reset:
        {
            drawReset(g, x, y, width);
            break;
        }
        case GlobalControlsLAF::Jog: { break; }
        case GlobalControlsLAF::Hold:
        {
            drawHold(g, x, y, width, state);
            break;
        }
    }
}

void GlobalControlsLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider)
{
    drawJog(g, x, y, width, sliderPosProportional);
}


void GlobalControlsLookAndFeel::drawNudge(juce::Graphics &g, float x, float y, float size, float rotation)
{
    juce::Path graphicPath;
    for (int i = 0; i < 5; i++)
    {
        float sizeIncr = (size/5) * (i * (1.0f - lpgValue));
        juce::Point<float> topPoint = { x + sizeIncr, y };
        juce::Point<float> botPoint = { x + sizeIncr, y + size };
        
        graphicPath.startNewSubPath(topPoint);
        graphicPath.lineTo(botPoint);
        
    }
    graphicPath.applyTransform(juce::AffineTransform::rotation(rotation, x + size/2, y + size/2));
    g.setColour(Colors::graphicBlackAlt);
    auto strokeType = juce::PathStrokeType(2.0f, juce::PathStrokeType::JointStyle::curved);
    strokeType.setEndStyle(juce::PathStrokeType::EndCapStyle::rounded);
    g.strokePath(graphicPath, strokeType);

}

void GlobalControlsLookAndFeel::drawBrake(juce::Graphics &g, float x, float y, float size)
{
    juce::Path topPath, bottomPath;
    float topHeight = size * 0.8f * lpgValue;
    float bottomHeight = size * 0.8f * (1.0f - lpgValue);

    topPath.addRoundedRectangle(x + size * 0.05f,
                                y,
                                size * 0.9f,
                                topHeight,
                                size * 0.1f);
    
    bottomPath.addRoundedRectangle(x + size * 0.05f,
                                   (y + size * 0.2f) + topHeight,
                                   size * 0.9f,
                                   bottomHeight,
                                   size * 0.1f);
    
    g.setColour(Colors::graphicBlackAlt);
    g.fillPath(topPath);
    g.strokePath(topPath, juce::PathStrokeType(2.0f));
    g.fillPath(bottomPath);
    g.strokePath(bottomPath, juce::PathStrokeType(2.0f));
}

void GlobalControlsLookAndFeel::drawReset(juce::Graphics &g, float x, float y, float size)
{
    juce::Rectangle<float> graphicBounds = {x, y + size * 0.05f, size, size * 0.9f };
    float reduceAmount = (size * 0.075f) + (lpgValue * size * 0.075f);
    graphicBounds.reduce(reduceAmount, reduceAmount);
    g.setColour(Colors::graphicBlackAlt);
    
    float triangleSize = size * 0.25f;
    juce::Path leftPath, middlePath;
    leftPath.startNewSubPath(graphicBounds.getTopLeft());
    leftPath.lineTo(graphicBounds.getBottomLeft());
    
    auto strokeType = juce::PathStrokeType(2.0f, juce::PathStrokeType::JointStyle::curved);
    strokeType.setEndStyle(juce::PathStrokeType::EndCapStyle::rounded);
    g.strokePath(leftPath, strokeType);

    middlePath.startNewSubPath(graphicBounds.getX() + triangleSize , graphicBounds.getCentreY());
    middlePath.lineTo(graphicBounds.getX() + graphicBounds.getWidth() , graphicBounds.getCentreY());
    drawTriangle(g, triangleSize,
                 { graphicBounds.getX() + triangleSize,
                graphicBounds.getCentreY() },
                 4.71f);
    
    g.strokePath(middlePath, strokeType);

}

void GlobalControlsLookAndFeel::drawTriangle(juce::Graphics& g, float size, juce::Point<float> endCoords, float rotation)
{
    juce::Path trianglePath;
    trianglePath.startNewSubPath(0.0f, -size / 2.0f);
    trianglePath.lineTo(size / 2.0f, size / 2.0f);
    trianglePath.lineTo(-size / 2.0f, size / 2.0f);
    trianglePath.closeSubPath();
    trianglePath = trianglePath.createPathWithRoundedCorners(1.0f);
    juce::AffineTransform transform = juce::AffineTransform::rotation(rotation).translated(endCoords);
    trianglePath.applyTransform(transform);
    g.fillPath(trianglePath);
}

void GlobalControlsLookAndFeel::drawJog(juce::Graphics &g, float x, float y, float size, float position)
{
    x = x + size * 0.1f;
    y = y + size * 0.1f;
    size *= 0.8f;

    g.setColour(Colors::graphicBlackAlt);

    juce::Path linePath, dotPath;
    linePath.addCentredArc(x + size,
                           y + size,
                           size,
                           size,
                           0.0f,
                           4.71f,
                           6.28f,
                           true);
    
    auto strokeType = juce::PathStrokeType(2.0f, juce::PathStrokeType::JointStyle::curved);
    strokeType.setEndStyle(juce::PathStrokeType::EndCapStyle::rounded);
    g.strokePath(linePath, strokeType);

    
    float dotRadius = size * 0.1f;
    float posScaled = 3.14f + position * 1.57;
    juce::Point dotCoords = { x + size + std::cos(posScaled) * size,
        y + size + std::sin(posScaled) * size };
    dotPath.addCentredArc(dotCoords.x,
                          dotCoords.y,
                          dotRadius,
                          dotRadius,
                          0.0f,
                          0.0f,
                          6.28f,
                          true);
    
    g.fillPath(dotPath);
}

void GlobalControlsLookAndFeel::drawHold(juce::Graphics &g, float x, float y, float size, bool state)
{
    for (int i = 0; i < 10; i++)
    {
        float sizeIncr = (size/10) * (i * (1.0f - lpgValue));
        juce::Point<float> topPoint = { x + sizeIncr, y };
        juce::Point<float> botPoint = { x + sizeIncr, y + size };
        
        juce::Path graphicPath;
        graphicPath.startNewSubPath(topPoint);
        graphicPath.lineTo(botPoint);
        
        if (state){
            float centerX = x + size * 0.25f;
            juce::AffineTransform mirrorTransform =
            juce::AffineTransform::translation(-centerX, 0.0f)
                .scaled(-1.0f, 1.0f)
                .translated(centerX, 0.0f);
            graphicPath.applyTransform(mirrorTransform);
        }
        
        g.setColour(Colors::graphicBlackAlt);
        g.strokePath(graphicPath, juce::PathStrokeType(2));
    }
}

void GlobalControlsLookAndFeel::setLPGValue(float lpgValue)
{
    this->lpgValue = lpgValue;
}





GlobalControlsLayout::GlobalControlsLayout(TingeAudioProcessor &p) : audioProcessor(p)
{
    // nudge
    setLabel(*this, nudgeLabel, "Nudge", Colors::graphicBlack, juce::Justification::centred);
    nudgeForwardButton = std::make_unique<GlobalHoldableButton>(audioProcessor, HoldableButtonLAF::NudgeForward);
    addAndMakeVisible(*nudgeForwardButton);
    
    nudgeBackwardButton = std::make_unique<GlobalHoldableButton>(audioProcessor, HoldableButtonLAF::NudgeBackward);
    addAndMakeVisible(*nudgeBackwardButton);

    setLabel(*this, brakeLabel, "Brake", Colors::graphicBlack, juce::Justification::centred);
    brakeButton = std::make_unique<GlobalHoldableButton>(audioProcessor, HoldableButtonLAF::Brake);
    addAndMakeVisible(*brakeButton);

    // jog
    setLabel(*this, jogLabel, "Jog", Colors::graphicBlack, juce::Justification::centred);
    setSlider(*this, jogSlider, jogLAF);
    jogAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params->apvts, "jog", jogSlider);

    // hold
    setLabel(*this, holdLabel, "Hold", Colors::graphicBlack, juce::Justification::centred);
    setButton(*this, holdButton, holdLAF);
    holdButton.addListener(this);
    holdButton.setToggleable(true);
    holdButton.setClickingTogglesState(true);
    holdButton.setToggleState(false, juce::dontSendNotification);

    // reset
    setLabel(*this, resetLabel, "Reset", Colors::graphicBlack, juce::Justification::centred);
    setButton(*this, resetButton, resetLAF);
    resetButton.addListener(this);
    
    // set up slew values
    resetSlew.setSampleRate(60);
    resetSlew.setEnvelopeSlew(240, 240);

    // set up timer
    startTimerHz(60);
}

GlobalControlsLayout::~GlobalControlsLayout()
{
    resetButton.removeListener(this);
}

void GlobalControlsLayout::resized()
{
    auto bounds = getLocalBounds();
    float x = bounds.getX();
    float y = bounds.getY();
    float width = bounds.getWidth();
    float height = bounds.getHeight();

    float buttonSize = height * 0.725f;
    float margin = width * 0.05f;
    
    // nudge
    nudgeLabel.setBounds(x + buttonSize/2,
                         height * 0.775f,
                         buttonSize * 2.0f,
                         getFontSize());
    
    nudgeBackwardButton->setBounds(x + buttonSize/2,
                                 y + height * 0.05f,
                                 buttonSize,
                                 buttonSize);
    
    nudgeForwardButton->setBounds((x + buttonSize/2) + buttonSize,
                                 y + height * 0.05f,
                                 buttonSize,
                                 buttonSize);

    // brake
    brakeLabel.setBounds((x + buttonSize/2) + buttonSize * 2.25f,
                         height * 0.775f,
                         buttonSize,
                         getFontSize());
    
    brakeButton->setBounds((x + buttonSize/2) + buttonSize * 2.25f,
                          y + height * 0.05f,
                          buttonSize,
                          buttonSize);

    // jog
    jogLabel.setBounds((x + buttonSize/2) + buttonSize * 3.5f,
                         height * 0.775f,
                         buttonSize,
                         getFontSize());

    
    jogSlider.setBounds((x + buttonSize/2) + buttonSize * 3.5f,
                        height * 0.05f,
                        buttonSize,
                        buttonSize);

    // hold
    holdLabel.setBounds(x + width - margin - buttonSize * 2.25f,
                         height * 0.775f,
                         buttonSize,
                         getFontSize());

    holdButton.setBounds(x + width - margin - buttonSize * 2.25f,
                          y + height * 0.05f,
                          buttonSize,
                          buttonSize);

    // reset
    resetLabel.setBounds(x + width - margin - buttonSize,
                         height * 0.775f,
                         buttonSize,
                         getFontSize());

    resetButton.setBounds(x + width - margin - buttonSize,
                          y + height * 0.05f,
                          buttonSize,
                          buttonSize);
}

void GlobalControlsLayout::buttonClicked(juce::Button* b)
{
    if (b == &holdButton)
    {
        bool currentHoldValue = (bool)audioProcessor.params->hold->get();
        bool hold = !currentHoldValue;
        audioProcessor.params->apvts.getParameter("hold")->setValueNotifyingHost(hold);
    }
}


void GlobalControlsLayout::buttonStateChanged(juce::Button* b)
{
    if (!b->isMouseButtonDown() && b->isMouseOver() && b->isDown())
    {
        return;
    }

     if (b == &resetButton)
    {
        float reset = b->isDown() ? 1.0f : 0.0f;
        resetSlew.triggerEnvelope(b->isDown());
        audioProcessor.params->apvts.getParameter("reset")->setValueNotifyingHost(reset);
        
    }
}

void GlobalControlsLayout::timerCallback()
{
    float reset = resetSlew.generateEnvelope();
    resetLAF.setLPGValue(reset);
    resetButton.repaint();
}
