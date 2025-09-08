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
    
    switch(lookAndFeel){
        case ThresholdLAF::Max:
        {
            drawThresholdMax(g, x, y, width, height, sliderPosProportional);
            break;
        }
        case ThresholdLAF::Phase:
        {
            drawThresholdPhase(g, x, y, width, sliderPosProportional);
            break;
        }
            
    }
}

void ThresholdLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    float x = bounds.getX();
    float y = bounds.getY();
    float width = bounds.getWidth();
    bool state = button.getToggleState();
    
    auto iconBounds = bounds;
    iconBounds.reduce(width * 0.2f, width * 0.2f);
    float iconX = iconBounds.getX();
    float iconY = iconBounds.getY();
    float iconWidth = iconBounds.getWidth();

    
    switch(lookAndFeel){
        case ThresholdLAF::Equidistant:
        {
            drawOverlapBackground(g, x, y, width, state, 0, 2);
            drawEquidistant(g, iconX, iconY, iconWidth, state);
            break;
        }
        case ThresholdLAF::Fill:
        {
            drawOverlapBackground(g, x, y, width, state, 1, 2);
            drawFill(g, iconX, iconY, iconWidth, state);
            break;
        }
        case ThresholdLAF::Harmonic:
        {
            drawOverlapBackground(g, x, y, width, state, 0, 1);
            drawHarmonic(g, iconX, iconY, iconWidth, state);
            break;
        }
        case ThresholdLAF::Random:
        {
            drawOverlapBackground(g, x, y, width, state, 0, 2);
            drawRandom(g, iconX, iconY, iconWidth, state);
            break;
        }
        case ThresholdLAF::Sequential:
        {
           // drawSequenial(g, x, y, width, sliderPosProportional);
            break;
        }

        case ThresholdLAF::Fibonacci:
        {
            drawOverlapBackground(g, x, y, width, state, 0, 2);
            drawFibonacci(g, iconX, iconY, iconWidth, state);
            break;
        }

    }
    
}




void ThresholdLookAndFeel::drawThresholdPhase(juce::Graphics &g, float x, float y, float size, float position)
{
    
}

void ThresholdLookAndFeel::drawThresholdMax(juce::Graphics &g, float x, float y, float width, float height, float position)
{
    juce::Path topPath, bottomPath;
    float graphicWidth = width/16;
    float graphicHeight = height * 0.5f;
    float graphicMargin = width * 0.025f;
    int positionIndex = std::floor(position * 15.0f);
    for (int i = 0; i < 16; i++)
    {
        bool indexBool = i <= positionIndex;
        int j = i + 1;
        float selectionOffset = indexBool ?  height * 0.045f : 0.0f;
        float heightOffset = std::sin(position * (i + 1)) * height * 0.15f;
        heightOffset = indexBool ? heightOffset : 0.0f;
        
        topPath.addRoundedRectangle(x + graphicMargin + (graphicWidth * i),
                                        (y + height * 0.25f) + heightOffset + selectionOffset,
                                        graphicWidth - graphicMargin,
                                        graphicHeight,
                                        (graphicWidth - graphicMargin) * 0.5f);
        
        bottomPath.addRoundedRectangle(x + graphicMargin + (graphicWidth * i),
                                        (y + height * 0.25f) + heightOffset - selectionOffset,
                                        graphicWidth - graphicMargin,
                                        graphicHeight,
                                        (graphicWidth - graphicMargin) * 0.5f);

        if (indexBool){
            g.setColour(Colors::primaryColor[0]);
            g.fillPath(topPath);
            
            g.setColour(Colors::primaryColor[1]);
            g.fillPath(bottomPath);
        }
        
        auto darkenedColor = Palette::darkenColors(Colors::primaryColor[0], Colors::primaryColor[1]);
        drawDoubleOverlap(g, topPath, bottomPath, darkenedColor, false);
    }
    
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

void ThresholdLookAndFeel::drawOverlapBackground(juce::Graphics &g, float x, float y, float size, bool state, int c1, int c2)
{
    juce::Rectangle<float> bgLeft = { x,
        y,
        size * 0.9f,
        size * 0.9f };
    juce::Rectangle<float> bgRight = { x + size * 0.1f,
        y + size * 0.1f,
        size * 0.9f,
        size * 0.9f };
    juce::Path bgLeftPath, bgRightPath;
    bgLeftPath.addRoundedRectangle(bgLeft, size * 0.075f);
    bgRightPath.addRoundedRectangle(bgRight, size * 0.075f);

    auto color1 = state ? Colors::primaryColor[c1] : Colors::graphicBlack;
    auto color2 = state ? Colors::primaryColor[c2] : Colors::graphicBlack;

    drawWithoutOverlap(g, bgLeftPath, color1, false);
    drawWithoutOverlap(g, bgRightPath, color2, false);
    
    auto darkenedColor = Palette::darkenColors(Colors::primaryColor[c1], Colors::primaryColor[c2]);
    auto color3 = state ? darkenedColor : Colors::graphicBlackAlt;

    drawDoubleOverlap(g, bgLeftPath, bgRightPath, color3, false);
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
        g.strokePath(graphicPath, juce::PathStrokeType(1.5));

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
        g.strokePath(topPath, juce::PathStrokeType(1.5f));
        g.strokePath(bottomPath, juce::PathStrokeType(1.5f));

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

void ThresholdLookAndFeel::drawSequential(juce::Graphics &g, float x, float y, float size, bool state)
{
    drawOverlapBackground(g, x, y, size, state, 0, 1);
    
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
    setSlider(*this, thresholdPhaseDial, phaseLAF);
    
    
    setLabel(*this, thresholdMaxLabel, "Threshold Max", juce::Justification::left);
    
    thresholdMaxTextSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, "maxThreshold");
    thresholdMaxTextSlider->setUnitStyle(UnitStyle::Int);
    thresholdMaxTextSlider->setJustification(juce::Justification::left);
    thresholdMaxTextSlider->setFontSize(12.0f);

    setSlider(*this, thresholdMaxDial, maxLAF);

    for (int i = 0; i < 5; i++)
    {
        setButton(*this, thresholdModeButton[i], thresholdModeLAF[i]);
        thresholdModeButton[i].setToggleable(true);
        thresholdModeButton[i].setClickingTogglesState(true);
        thresholdModeButton[i].setRadioGroupId(1);
    }
 

}
ThresholdLayout::~ThresholdLayout()
{
    for (int i = 0; i < 5; i++)
    {
        thresholdModeButton[i].setLookAndFeel(nullptr);
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
    
    thresholdPhaseDial.setBounds(x + width * 0.1f,
                                 y + height * 0.25f,
                                 width * 0.35f,
                                 width * 0.35f);
    
    thresholdMaxLabel.setBounds(x + width * 0.1f,
                                 y,
                                 width * 0.8f,
                                 height * 0.1f);

    thresholdMaxTextSlider->setBounds(x + width * 0.5f,
                                 y,
                                 width * 0.5f,
                                 height * 0.1f);

    thresholdMaxDial.setBounds(x + width * 0.1f,
                                 y + height * 0.1f,
                                 width * 0.8f,
                                 height * 0.1f);
     
    /*
    thresholdModeButton[0].setBounds(x,
                                 y + height * 0.1f,
                                 width * 0.45f,
                                 width * 0.45f);
    
    thresholdModeButton[1].setBounds(x + width * 0.5f,
                                 y + height * 0.1f,
                                 width * 0.45f,
                                 width * 0.45f);
     

    thresholdModeButton[2].setBounds(x,
                                 y + height * 0.3f,
                                 width * 0.45f,
                                 width * 0.45f);

    thresholdModeButton[3].setBounds(x + width * 0.5f,
                                 y + height * 0.3f,
                                 width * 0.45f,
                                 width * 0.45f);

    thresholdModeButton[4].setBounds(x,
                                 y + height * 0.5f,
                                 width * 0.45f,
                                 width * 0.45f);
    */
}
