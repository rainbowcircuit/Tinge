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
    float height = bounds.getHeight();

    float state = button.getToggleState();
    switch(lookAndFeel)
    {
        case GlobalControlsLAF::NudgeForward:
        {
            drawNudge(g, x, y, width, true);
            break;
        }
        case GlobalControlsLAF::NudgeBackward:
        {
            drawNudge(g, x, y, width, false);
            break;
        }
        case GlobalControlsLAF::Brake:
        {
            drawBrake(g, x, y, width, height);
            break;
        }
        case GlobalControlsLAF::Reset:
        {
            drawReset(g, x, y, width);
            break;
        }
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


void GlobalControlsLookAndFeel::drawNudge(juce::Graphics &g, float x, float y, float size, bool drawNudgeBack)
{
    for (int i = 0; i < 5; i++)
    {
        float sizeIncr = (size/5) * (i * (1.0f - lpgValue));
        juce::Point<float> topPoint = { x + sizeIncr, y };
        juce::Point<float> botPoint = { x + sizeIncr, y + size };
        
        juce::Path graphicPath;
        graphicPath.startNewSubPath(topPoint);
        graphicPath.lineTo(botPoint);
        
        if (drawNudgeBack)
        {
            float centerX = x + size * 0.5f;
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

void GlobalControlsLookAndFeel::drawBrake(juce::Graphics &g, float x, float y, float width, float height)
{
    float radius = height * 0.5f;
    float offset = (0.5f - lpgValue * 0.5f) * radius;

    juce::Path leftCircle, rightCircle;
    juce::Point centerCoords = { x + width/2, y + height/2 };
    
    leftCircle.addCentredArc(centerCoords.x - offset,
                             centerCoords.y,
                             radius,
                             radius,
                             0.0f,
                             0.0f,
                             6.28f,
                             true);
    
    rightCircle.addCentredArc(centerCoords.x + offset,
                              centerCoords.y,
                              radius,
                              radius,
                              0.0f,
                              0.0f,
                              6.28f,
                              true);
    
    g.setColour(Colors::graphicBlackAlt);
    g.fillPath(leftCircle);
    g.fillPath(rightCircle);

    drawDoubleOverlap(g, leftCircle, rightCircle, Colors::backgroundFill, true);
    g.strokePath(leftCircle, juce::PathStrokeType(2));
    g.strokePath(rightCircle, juce::PathStrokeType(2));

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
    g.strokePath(leftPath, juce::PathStrokeType(2));

    middlePath.startNewSubPath(graphicBounds.getX() + triangleSize , graphicBounds.getCentreY());
    middlePath.lineTo(graphicBounds.getX() + graphicBounds.getWidth() , graphicBounds.getCentreY());
    drawTriangle(g, triangleSize,
                 { graphicBounds.getX() + triangleSize,
                graphicBounds.getCentreY() },
                 4.71f);

    g.strokePath(middlePath, juce::PathStrokeType(2));

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
    float pi = juce::MathConstants<float>::pi;
    float dialStart = 1.25f * pi;
    float dialEnd = 2.75f * pi;
    float sliderPositionScaled = 2.0f + (1.0f - position);
    float dialPositionInRadians = dialStart + sliderPositionScaled * (dialEnd - dialStart);
    
    juce::Path dialBodyPath, dialDotPath, dialOutlinePath;
    float dialOutlineRadius = (size * 0.9f)/2;
    float dialBodyRadius = (size * 0.75f)/2;
    float dialDotRadius = (size * 0.45f)/2;

    dialOutlinePath.addCentredArc(x + size/2, y + size/2,
                                  dialOutlineRadius, dialOutlineRadius,
                               0.0f, dialStart, dialEnd, true);
    g.setColour(Colors::graphicBlackAlt);
    g.strokePath(dialOutlinePath, juce::PathStrokeType(1.5f));
    
    //==============================================================================
    // dial body
    dialBodyPath.addCentredArc(x + size/2, y + size/2,
                               dialBodyRadius, dialBodyRadius,
                               0.0f, 0.0f, 6.28f, true);
    g.setColour(Colors::graphicBlackAlt);
    g.fillPath(dialBodyPath);

    //==============================================================================
    // dial dot
    juce::Point<float> outlineCoords = {x + size/2 + std::sin(dialPositionInRadians) * dialDotRadius,
        x + size/2 + std::cos(dialPositionInRadians) * dialDotRadius};
    juce::Point<float> startCoords = {x + size/2 + std::sin(dialStart) * dialDotRadius,
        x + size/2 + std::cos(dialStart) * dialDotRadius};

    dialDotPath.addCentredArc(outlineCoords.x, outlineCoords.y,
                              1.5, 1.5, 0.0f, 0.0f, pi * 2, true);
    
    g.setColour(Colors::backgroundFill);
    g.fillPath(dialDotPath);
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
    
    setLabel(*this, nudgeLabel, "Nudge", juce::Justification::centred);
    setButton(*this, nudgeForwardButton, nudgeForwardLAF);
    nudgeForwardButton.addListener(this);

    setButton(*this, nudgeBackwardButton, nudgeBackwardLAF);
    nudgeBackwardButton.addListener(this);

    setLabel(*this, brakeLabel, "Brake", juce::Justification::centred);
    setButton(*this, brakeButton, brakeLAF);
    brakeButton.addListener(this);

    setLabel(*this, jogLabel, "Jog", juce::Justification::centred);
    setSlider(*this, jogSlider, jogLAF);
    
    setLabel(*this, resetLabel, "Reset", juce::Justification::centred);
    setButton(*this, resetButton, resetLAF);
    resetButton.addListener(this);
    
 //   setLabel(*this, resetLabel, "Reset", juce::Justification::centred);
    setButton(*this, holdButton, holdLAF);
    holdButton.addListener(this);
    holdButton.setToggleable(true);
    holdButton.setClickingTogglesState(true);
    holdButton.setToggleState(false, juce::dontSendNotification);
    
    nudgeForwardSlew.setSampleRate(60);
    nudgeForwardSlew.setEnvelopeSlew(240, 240);
    nudgeBackwardSlew.setSampleRate(60);
    nudgeBackwardSlew.setEnvelopeSlew(240, 240);
    brakeSlew.setSampleRate(60);
    brakeSlew.setEnvelopeSlew(240, 240);
    resetSlew.setSampleRate(60);
    resetSlew.setEnvelopeSlew(240, 240);

    startTimerHz(60);
}

GlobalControlsLayout::~GlobalControlsLayout()
{
    nudgeForwardButton.removeListener(this);
    nudgeBackwardButton.removeListener(this);
    brakeButton.removeListener(this);
    resetButton.removeListener(this);
}


void GlobalControlsLayout::resized()
{
    auto bounds = getLocalBounds();
    float x = bounds.getX();
    float y = bounds.getY();
    float width = bounds.getWidth();
    float height = bounds.getHeight();

    float buttonSize = height * 0.75f;
    float margin = width * 0.05f;
    
    // nudge
    nudgeLabel.setBounds(x + buttonSize/2,
                         height * 0.775f,
                         buttonSize * 2.0f,
                         getFontSize());
    
    nudgeBackwardButton.setBounds(x + buttonSize/2,
                                 y + height * 0.05f,
                                 buttonSize,
                                 buttonSize);
    
    nudgeForwardButton.setBounds((x + buttonSize/2) + buttonSize,
                                 y + height * 0.05f,
                                 buttonSize,
                                 buttonSize);

    // brake
    brakeLabel.setBounds((x + buttonSize/2) + buttonSize * 2.0f,
                         height * 0.775f,
                         buttonSize * 2,
                         getFontSize());
    
    brakeButton.setBounds((x + buttonSize/2) + buttonSize * 2.0f,
                          y + height * 0.05f,
                          buttonSize * 2,
                          buttonSize);

    // jog
    jogLabel.setBounds((x + buttonSize/2) + buttonSize * 4.0f,
                         height * 0.775f,
                         buttonSize,
                         getFontSize());

    
    jogSlider.setBounds((x + buttonSize/2) + buttonSize * 4.0f,
                        height * 0.05f,
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
    
    holdButton.setBounds(x + width - margin - buttonSize * 2,
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
    if (b == &nudgeForwardButton) {
        
        float nudgeValue = b->isDown() ? 1.0f : 0.0f;
        nudgeForwardSlew.triggerEnvelope(b->isDown());
        audioProcessor.params->apvts.getParameter("nudgeForward")->setValueNotifyingHost(nudgeValue);
        
    } else if (b == &nudgeBackwardButton)
    {
        float nudgeValue = b->isDown() ? 1.0f : 0.0f;
        nudgeBackwardSlew.triggerEnvelope(b->isDown());
        audioProcessor.params->apvts.getParameter("nudgeBackward")->setValueNotifyingHost(nudgeValue);
        
    } else if (b == &brakeButton)
    {
        float brake = b->isDown() ? 1.0f : 0.0f;
        brakeSlew.triggerEnvelope(b->isDown());
        audioProcessor.params->apvts.getParameter("brake")->setValueNotifyingHost(brake);
        
    } else if (b == &resetButton)
    {
        float reset = b->isDown() ? 1.0f : 0.0f;
        resetSlew.triggerEnvelope(b->isDown());
        audioProcessor.params->apvts.getParameter("reset")->setValueNotifyingHost(reset);
        
    }
}

void GlobalControlsLayout::timerCallback()
{
    float nudgeForward = nudgeForwardSlew.generateEnvelope();
    nudgeForwardLAF.setLPGValue(nudgeForward);
    nudgeForwardButton.repaint();
    
    float nudgeBackward = nudgeBackwardSlew.generateEnvelope();
    nudgeBackwardLAF.setLPGValue(nudgeBackward);
    nudgeBackwardButton.repaint();

    float brake = brakeSlew.generateEnvelope();
    brakeLAF.setLPGValue(brake);
    brakeButton.repaint();
    
    float reset = resetSlew.generateEnvelope();
    resetLAF.setLPGValue(reset);
    resetButton.repaint();
}
