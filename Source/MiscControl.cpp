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
    }
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
        
        g.setColour(juce::Colour(40, 40, 40));
        g.strokePath(graphicPath, juce::PathStrokeType(2));
    }
}

void GlobalControlsLookAndFeel::drawBrake(juce::Graphics &g, float x, float y, float width, float height)
{
    float radius = height * 0.5f;
    float offset = (0.5f - lpgValue * 0.5f) * radius;

    juce::Path leftCircle, rightCircle;
    juce::Point centerCoords = { x + width/2, y + height/2 };
    
    leftCircle.addCentredArc(centerCoords.x - offset, centerCoords.y, radius, radius, 0.0f, 0.0f, 6.28f, true);
    rightCircle.addCentredArc(centerCoords.x + offset, centerCoords.y, radius, radius, 0.0f, 0.0f, 6.28f, true);
    g.setColour(juce::Colour(40, 40, 40));
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
    g.setColour(juce::Colour(40, 40, 40));
    
    float triangleSize = size * 0.25f;
    juce::Path leftPath, rightPath;
    leftPath.startNewSubPath(graphicBounds.getBottomLeft());
    leftPath.lineTo(graphicBounds.getTopLeft());
    leftPath.lineTo(graphicBounds.getTopRight().x - triangleSize/2, graphicBounds.getTopRight().y);
    leftPath = leftPath.createPathWithRoundedCorners(size * 0.15f);

    
    drawTriangle(g, triangleSize, {graphicBounds.getTopRight().x - triangleSize/2, graphicBounds.getTopRight().y }, 1.57f);
    g.strokePath(leftPath, juce::PathStrokeType(2));
    
    rightPath.startNewSubPath(graphicBounds.getTopRight().x,
                              graphicBounds.getTopRight().y + triangleSize);
    rightPath.lineTo(graphicBounds.getBottomRight());
    rightPath.lineTo(graphicBounds.getBottomLeft().x + triangleSize,
                     graphicBounds.getBottomLeft().y);
    rightPath = rightPath.createPathWithRoundedCorners(size * 0.15f);
    
    drawTriangle(g, triangleSize,
                 { graphicBounds.getBottomLeft().x + triangleSize,
                graphicBounds.getBottomLeft().y },
                 4.71f);

    g.strokePath(rightPath, juce::PathStrokeType(2));

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

    setLabel(*this, resetLabel, "Reset", juce::Justification::centred);
    setButton(*this, resetButton, resetLAF);
    resetButton.addListener(this);
    
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
    auto bounds = getLocalBounds().toFloat();
    float x = bounds.getX();
    float y = bounds.getY();
    float width = bounds.getWidth();
    float height = bounds.getHeight();

    float buttonSize = height * 0.75f;
    
    nudgeLabel.setBounds(x + width * 0.05f,
                         height * 0.75f,
                         width * 0.15f,
                         height * 0.25f);
    
    nudgeBackwardButton.setBounds(x + width * 0.05f,
                                 y + height * 0.05f,
                                 buttonSize,
                                 buttonSize);
    
    nudgeForwardButton.setBounds(x + width * 0.2f,
                                 y + height * 0.05f,
                                 buttonSize,
                                 buttonSize);

    brakeLabel.setBounds(x + width * 0.4f,
                         height * 0.75f,
                         width * 0.15f,
                         height * 0.25f);

    brakeButton.setBounds(x + width * 0.4f,
                          y + height * 0.05f,
                          buttonSize * 2,
                          buttonSize);

    resetLabel.setBounds(x + width * 0.6f,
                         height * 0.75f,
                         width * 0.15f,
                         height * 0.25f);

    resetButton.setBounds(x + width * 0.6f,
                          y + height * 0.05f,
                          buttonSize,
                          buttonSize);
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
        float nudgeValue = b->isDown() ? 1.0f : 0.0f;
        brakeSlew.triggerEnvelope(b->isDown());
        audioProcessor.params->apvts.getParameter("brake")->setValueNotifyingHost(nudgeValue);
        
    } else if (b == &resetButton)
    {
        float nudgeValue = b->isDown() ? 1.0f : 0.0f;
        resetSlew.triggerEnvelope(b->isDown());
        audioProcessor.params->apvts.getParameter("reset")->setValueNotifyingHost(nudgeValue);
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
