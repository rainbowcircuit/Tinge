/*
  ==============================================================================

    ThresholdControl.cpp
    Created: 31 Aug 2025 9:57:38am
    Author:  Takuma Matsui

  ==============================================================================
*/

#include "ThresholdControl.h"

void ThresholdLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider)
{
    this->lineWidth = width * 0.02f;
    
    bool hover = slider.isMouseOver();
    graphicGrey = !hover ? Colors::graphicGrey : Palette::addFloor(Colors::graphicGrey, 0.05f);

    juce::Rectangle<float> bounds = { (float)x, (float)y, (float)width, (float)height };
    bounds.reduce(width * 0.05f, width * 0.05f);
    
    switch(lookAndFeel){
        case ThresholdLAF::Max:
        {
            drawThresholdMax(g, bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), sliderPosProportional);
            break;
        }
        case ThresholdLAF::Phase:
        {
            drawThresholdPhase(g, bounds.getX(), bounds.getY(), bounds.getWidth(), 0.0f, true);
            drawThresholdPhase(g, bounds.getX(), bounds.getY(), bounds.getWidth(), sliderPosProportional, false);
            break;
        }
        case ThresholdLAF::ValueSlew:
        {
            drawValueSlew(g, bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), sliderPosProportional);
            break;
        }
        case ThresholdLAF::Equidistant: { break; }
        case ThresholdLAF::Fill: { break; }
        case ThresholdLAF::Harmonic: { break; }
        case ThresholdLAF::Random: { break; }
        case ThresholdLAF::Fibonacci: { break; }
    }
}

void ThresholdLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    float x = bounds.getX();
    float y = bounds.getY();
    float width = bounds.getWidth();
    
    
    
    bool hover = button.isMouseOver();
    graphicGrey = !hover ? Colors::graphicGrey : Palette::addFloor(Colors::graphicGrey, 0.05f);

    bool state = button.getToggleState();
    float alpha = state ? 1.0f : 0.25f;
    
    g.setColour(graphicGrey.withAlpha((float)alpha));
    g.fillRoundedRectangle(x, y, width, width, width * 0.15f);

    
    auto iconBounds = bounds;
    iconBounds.reduce(width * 0.2f, width * 0.2f);
    float iconX = iconBounds.getX();
    float iconY = iconBounds.getY();
    float iconWidth = iconBounds.getWidth();

    
    

    switch(lookAndFeel){
        case ThresholdLAF::Max: { break; }
        case ThresholdLAF::Phase: { break; }
        case ThresholdLAF::ValueSlew: { break; }
        case ThresholdLAF::Equidistant:
        {
            drawEquidistant(g, iconX, iconY, iconWidth, state);
            break;
        }
        case ThresholdLAF::Fill:
        {
            drawFill(g, iconX, iconY, iconWidth, state);
            break;
        }
        case ThresholdLAF::Harmonic:
        {
            drawHarmonic(g, iconX, iconY, iconWidth, state);
            break;
        }
        case ThresholdLAF::Random:
        {
            drawRandom(g, iconX, iconY, iconWidth, state);
            break;
        }
        case ThresholdLAF::Fibonacci:
        {
            drawFibonacci(g, iconX, iconY, iconWidth, state);
            break;
        }
    }
    
}




void ThresholdLookAndFeel::drawThresholdPhase(juce::Graphics &g, float x, float y, float size, float phase, bool isBackground)
{
    juce::Path graphicPath;
    graphicPath.startNewSubPath(x, y + size/2);
    
    float twopi = juce::MathConstants<float>::twoPi;
    int domainResolution = 128;
    float widthIncrement = size/domainResolution;
    float phaseScale = domainResolution/twopi;
    phase = phase * twopi;

    float freq = 2.0f;
    
    float sinStart = std::sin((0/phaseScale) * freq + phase);
    graphicPath.startNewSubPath(x + widthIncrement * 0, (y + size/2) + (size * sinStart/2));

    for (int i = 1; i < domainResolution; i++)
    {
        float sin = std::sin((i/phaseScale) * freq + phase);
        graphicPath.lineTo(x + widthIncrement * i, (y + size/2) + (size * sin/2));
    }

    graphicPath = graphicPath.createPathWithRoundedCorners(4.0f);
    
    auto alpha = isBackground ? 0.25f : 1.0f;
    g.setColour(graphicGrey.withAlpha((float)alpha));
    auto strokeType = juce::PathStrokeType(1.5f, juce::PathStrokeType::JointStyle::curved);
    strokeType.setEndStyle(juce::PathStrokeType::EndCapStyle::rounded);
    g.strokePath(graphicPath, strokeType);

}

void ThresholdLookAndFeel::drawThresholdMax(juce::Graphics &g, float x, float y, float width, float height, float position)
{
    juce::Path frontPath, backPath;
    float graphicWidth = width * 0.5;
    float graphicHeight = height/16;
    float graphicMargin = height * 0.025f;
    
    int positionIndex = std::floor(position * 15.0f);
    for (int i = 0; i < 16; i++)
    {
        bool indexBool = i >= positionIndex;
        
        backPath.addRoundedRectangle(x + width * 0.25f,
                                    y + graphicHeight * i,
                                    graphicWidth,
                                    graphicHeight - graphicMargin,
                                    graphicHeight/2);
        
        float xOffset = indexBool ? 0.0f : (std::sin(position * i) * width * 0.15f);
        
        frontPath.addRoundedRectangle(x + width * 0.25f + xOffset,
                                      y + graphicHeight * i,
                                      graphicWidth,
                                      graphicHeight - graphicMargin,
                                      graphicHeight/2);
        
    }
    g.setColour(graphicGrey.withAlpha((float)0.25f));
    g.fillPath(backPath);

    g.setColour(graphicGrey);
    g.fillPath(frontPath);

}

void ThresholdLookAndFeel::drawValueSlew(juce::Graphics &g, float x, float y, float width, float height, float position)
{
    juce::Path linePath, fillPath;
    juce::Point<float> botLeftCoords = { x + width * 0.1f, y + height * 0.9f };
    juce::Point<float> topRightCoords = { x + width * 0.9f, y + height * 0.1f };

    juce::Point<float> topMiddleCoords = { x + width * 0.5f, y + height * 0.1f };
    juce::Point<float> botMiddleCoords = { x + width * 0.5f, y + height * 0.9f };
    float triangleSize = width * 0.5f * position;

    
    linePath.startNewSubPath(botLeftCoords);
    linePath.lineTo(botMiddleCoords.x - triangleSize/2, botMiddleCoords.y );
    linePath.lineTo(topMiddleCoords.x + triangleSize/2, topMiddleCoords.y );
    linePath.lineTo(topRightCoords);

    
    fillPath.addTriangle(topMiddleCoords.x + triangleSize/2,
                         topMiddleCoords.y,
                         botMiddleCoords.x + triangleSize/2,
                         botMiddleCoords.y,
                         botMiddleCoords.x - triangleSize/2,
                         botMiddleCoords.y);
    fillPath = fillPath.createPathWithRoundedCorners(width * 0.05f);
    
    g.setColour(graphicGrey.withAlpha((float)0.25f));
    g.fillPath(fillPath);

    g.setColour(graphicGrey);
    auto strokeType = juce::PathStrokeType(2.0f, juce::PathStrokeType::JointStyle::curved);
    strokeType.setEndStyle(juce::PathStrokeType::EndCapStyle::rounded);
    g.strokePath(linePath, strokeType);
}
                                            
void ThresholdLookAndFeel::drawEquidistant(juce::Graphics &g, float x, float y, float size, bool state)
{
    juce::Point<float> centerCoords = { x + size/2, y + size/2 };
    float triangleSize = size * 0.85f;
    
    juce::Point<float> topCoords = { centerCoords.x, centerCoords.y - triangleSize/2 };
    juce::Point<float> botCoords = { centerCoords.x, centerCoords.y + triangleSize/2 };
    juce::Point<float> topLeftCoords = { centerCoords.x - triangleSize/2, botCoords.y - triangleSize };
    juce::Point<float> botLeftCoords = { centerCoords.x - triangleSize/2, topCoords.y + triangleSize };
    juce::Point<float> topRightCoords = { centerCoords.x + triangleSize/2, botCoords.y - triangleSize };
    juce::Point<float> botRightCoords = { centerCoords.x + triangleSize/2, topCoords.y + triangleSize };

    juce::Path graphicPath;
    graphicPath.startNewSubPath(topCoords);
    graphicPath.lineTo(topLeftCoords);
    graphicPath.lineTo(botLeftCoords);
    graphicPath.lineTo(botCoords);
    graphicPath.lineTo(botRightCoords);
    graphicPath.lineTo(topRightCoords);
    graphicPath.closeSubPath();
    
    g.setColour(Colors::graphicWhite);
    g.strokePath(graphicPath, juce::PathStrokeType(1.5));
    
}

void ThresholdLookAndFeel::drawFill(juce::Graphics &g, float x, float y, float size, bool state)
{
    float pi = juce::MathConstants<float>::pi;
    float startRadius = size * 0.3f;
    float endRadius = size * 0.45f;
    float startRadian = -pi/2.0f;
    
    juce::Path graphicPath;
    
    for (int i = 0; i <= 8; i++)
    {
        float radiusIncr = startRadian + (pi/8.0f) * i;
        
        juce::Point<float> centerCoords = { x + size/2, y + size/2};
        juce::Point<float> startCoords = { centerCoords.x + std::cos(radiusIncr) * startRadius,
            centerCoords.y + std::sin(radiusIncr) * startRadius };
        
        juce::Point<float> endCoords = { centerCoords.x + std::cos(radiusIncr) * endRadius,
            centerCoords.y + std::sin(radiusIncr) * endRadius };

        graphicPath.startNewSubPath(startCoords);
        graphicPath.lineTo(endCoords);
        
        float alpha = 1.0f/(i + 1.0f);
        g.setColour(Colors::graphicWhite.withAlpha((float)alpha));
        auto strokeType = juce::PathStrokeType(1.5f, juce::PathStrokeType::JointStyle::curved);
        strokeType.setEndStyle(juce::PathStrokeType::EndCapStyle::rounded);
        g.strokePath(graphicPath, strokeType);

    }
}

void ThresholdLookAndFeel::drawHarmonic(juce::Graphics &g, float x, float y, float size, bool state)
{
    juce::Path topPath, bottomPath;

    float twopi = juce::MathConstants<float>::twoPi;
    int domainResolution = 64;
    float widthIncrement = size/domainResolution;
    float phaseScale = domainResolution/twopi;
    
    for (int i = 1; i <= 3; i++){
        float freq = 0.5 * i;
        float yIncr = size * 0.3f * (i - 1);
        float yOffset = y + size * 0.2f;
        
        topPath.startNewSubPath(x, yOffset + yIncr);
        bottomPath.startNewSubPath(x, yOffset + yIncr);

        for (int j = 1; j < domainResolution; j++)
        {
            float sin = std::sin((j/phaseScale) * freq);
            topPath.lineTo(x + widthIncrement * j, (yOffset + yIncr) + (size * 0.075f * sin));
            bottomPath.lineTo(x + widthIncrement * j, (yOffset + yIncr) + (size * 0.075f * -sin));

        }
        topPath = topPath.createPathWithRoundedCorners(4.0f);
        bottomPath = bottomPath.createPathWithRoundedCorners(4.0f);

        float alpha = 1.0f - (1.0f/i);
        g.setColour(Colors::graphicWhite.withAlpha((float)alpha));
        auto strokeType = juce::PathStrokeType(1.5f, juce::PathStrokeType::JointStyle::curved);
        strokeType.setEndStyle(juce::PathStrokeType::EndCapStyle::rounded);
        g.strokePath(topPath, strokeType);
        g.strokePath(bottomPath, strokeType);
    }
}

void ThresholdLookAndFeel::drawRandom(juce::Graphics &g, float x, float y, float size, bool state)
{
    float graphicSize = size * 0.6f;
    float margin = size * 0.15;
    float radius = size * 0.075f;

    for (int i = 0; i < 25; i++){
        float xIncr = x + margin + (graphicSize/5) * (i % 5);
        float yIncr = y + margin + (graphicSize/5) * (i / 5);
        
        if (i % 3 == 0 || i % 7 == 0){
            g.setColour(Colors::graphicBlack);
        } else {
            g.setColour(Colors::graphicWhite);
        }
        g.fillEllipse(xIncr, yIncr, radius, radius);
    }
}

void ThresholdLookAndFeel::drawFibonacci(juce::Graphics &g, float x, float y, float size, bool state)
{
     // Fuck you a for loop would've taken a whole day.
    float scale = size/21;
    x += scale * 4;
    std::array<int, 7> fib = { 1, 1, 2, 3, 5, 8, 13 };
    float alphaIncr = 1.0f/7.0f;
    
    drawFibonacciCube(g,
                      x,
                      y + fib[5] * scale,
                      fib[6] * scale,
                      0.0f,
                      alphaIncr * 7);
    
    drawFibonacciCube(g,
                      x + fib[4] * scale,
                      y, fib[5] * scale,
                      4.71f,
                      alphaIncr * 6);
    
    drawFibonacciCube(g,
                      x,
                      y,
                      fib[4] * scale,
                      3.14f,
                      alphaIncr * 5);
    
    drawFibonacciCube(g,
                      x,
                      y + fib[4] * scale,
                      fib[3] * scale,
                      1.57f,
                      alphaIncr * 4);
    
    drawFibonacciCube(g,
                      x + fib[3] * scale,
                      y + fib[4] * scale,
                      fib[2] * scale,
                      4.71f,
                      alphaIncr * 3);
    
    drawFibonacciCube(g,
                      x + (fib[3] + fib[0]) * scale,
                      y + (fib[4] + fib[2]) * scale,
                      fib[1] * scale,
                      0.0f,
                      alphaIncr * 2);
    
    drawFibonacciCube(g,
                      x + fib[3] * scale,
                      y + (fib[4] + fib[2]) * scale,
                      fib[0] * scale, 1.57f,
                      alphaIncr);


}
void ThresholdLookAndFeel::drawFibonacciCube(juce::Graphics &g, float x, float y, float size, float rotation, float alpha)
{
    juce::Path graphicPath;
    
    graphicPath.addCentredArc(x, y, size, size, 0.0f, 1.57f, 3.14f, true);
    graphicPath.lineTo(x, y);
    graphicPath.closeSubPath();
    
    juce::AffineTransform rot = juce::AffineTransform::rotation(rotation, x + size/2, y + size/2);
    graphicPath.applyTransform(rot);
    g.setColour(Colors::graphicWhite.withAlpha((float)alpha));
    g.fillPath(graphicPath);
}


ThresholdLayout::ThresholdLayout(TingeAudioProcessor &p) : audioProcessor(p)
{
    
    setLabel(*this, thresholdPhaseLabel, "Phase", juce::Justification::centred);
    setSlider(*this, thresholdPhaseDial, phaseLAF);
    thresholdPhaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params->apvts, "thresholdPhase", thresholdPhaseDial);

    
    // max threshold
    setLabel(*this, thresholdMaxLabel, "Max", juce::Justification::centred);
    thresholdMaxTextSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, "maxThreshold");
    addAndMakeVisible(*thresholdMaxTextSlider);
    thresholdMaxTextSlider->setUnitStyle(UnitStyle::Int);
    thresholdMaxTextSlider->setJustification(juce::Justification::left);
    thresholdMaxTextSlider->setFontSize(12.0f);

    setSlider(*this, thresholdMaxDial, maxLAF);
    thresholdMaxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params->apvts, "maxThreshold", thresholdMaxDial);
    

    setLabel(*this, valueSlewLabel, "Slew", juce::Justification::centred);
    setSlider(*this, valueSlewDial, valueSlewLAF);
    valueSlewAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params->apvts, "valueSlew", valueSlewDial);

    
    // graphics
    addAndMakeVisible(graphics);
    
    
    for (int i = 0; i < 5; i++)
    {
        setButton(*this, thresholdModeButton[i], thresholdModeLAF[i]);
        thresholdModeButton[i].setToggleable(true);
        thresholdModeButton[i].setClickingTogglesState(true);
        thresholdModeButton[i].setRadioGroupId(1);
        thresholdModeButton[i].addListener(this);
    }
 

}
ThresholdLayout::~ThresholdLayout()
{
    for (int i = 0; i < 5; i++)
    {
        thresholdModeButton[i].setLookAndFeel(nullptr);
        thresholdModeButton[i].removeListener(this);

    }
}


void ThresholdLayout::resized()
{
    auto bounds = getLocalBounds().toFloat();
    float margin = bounds.getWidth() * 0.05f;
    bounds.reduce(margin, margin);
    float x = bounds.getX();
    float y = bounds.getY();
    float width = bounds.getWidth();
    float height = bounds.getHeight();
    
    float iconSize = width * 0.25f;
    // phase
    thresholdPhaseLabel.setBounds(x,
                                 y,
                                 iconSize,
                                 height * 0.05f);

    thresholdPhaseDial.setBounds(x,
                                 y + height * 0.05f,
                                 iconSize,
                                 iconSize);
        
    // max threshold
    thresholdMaxLabel.setBounds(x + width/2 - (iconSize/2),
                                y,
                                iconSize,
                                height * 0.05f);
    /*
    thresholdMaxTextSlider->setBounds(x + width/2 - (iconSize/2),
                                      y,
                                      iconSize,
                                      iconSize);
     */
    thresholdMaxDial.setBounds(x + width/2 - (iconSize/2),
                               y + height * 0.05f,
                               iconSize,
                               iconSize);
     
    
    // value slew
    valueSlewLabel.setBounds(x + width - iconSize,
                             y,
                             iconSize,
                             height * 0.05f);

    valueSlewDial.setBounds(x + width - iconSize,
                               y + height * 0.05f,
                               iconSize,
                               iconSize);

    // graphics
    graphics.setBounds(x,
                       y + height * 0.25f,
                       width,
                       width);

    
    float buttonSize = width * 0.2f;
    float buttonMargin = buttonSize * 0.1f;

    for (int i = 0; i < 5; i++)
    {
        thresholdModeButton[i].setBounds(x + buttonMargin + buttonSize * i,
                                         y + height * 0.7f,
                                         buttonSize - buttonMargin,
                                         buttonSize - buttonMargin);
    }
    
}

void ThresholdLayout::buttonClicked(juce::Button* b)
{
    auto param = audioProcessor.params->apvts.getParameter("thresholdMode");

    for (int i = 0; i < 5; i++)
    {
        if (b == &thresholdModeButton[i])
        {
            float valueNormalized = param->getNormalisableRange().convertTo0to1(i);
            param->setValueNotifyingHost(valueNormalized);
        }
    }
}
