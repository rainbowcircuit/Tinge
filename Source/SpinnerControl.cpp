/*
  ==============================================================================

    SpinnerControl.cpp
    Created: 19 Aug 2025 9:05:52pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#include "SpinnerControl.h"

void SpinnerControlsLookAndFeel::setIndex(int index)
{
    spinnerIndex = index;
}

void SpinnerControlsLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider)
{
    auto bounds = slider.getLocalBounds().toFloat();
    bounds.reduce(5, 5);
    float xPos = bounds.getX();
    float yPos = bounds.getY();
    float size = bounds.getWidth();
    float graphicWidth = bounds.getWidth();
    float graphicHeight = bounds.getHeight();
    
    switch(lookAndFeel){
        case SpinnerControlsLAF::Rate:
        {
            drawRate(g, xPos, yPos, graphicWidth, graphicHeight, sliderPosProportional);
            break;
        }
        case SpinnerControlsLAF::Ratio:
        {
            drawRatio(g, xPos, yPos, size, sliderPosProportional);
            break;
        }
        case SpinnerControlsLAF::Opacity:
        {
            drawOpacity(g, xPos, yPos, graphicWidth, graphicHeight, sliderPosProportional);
            break;
        }
    }
}

void SpinnerControlsLookAndFeel::drawRate(juce::Graphics& g, float x, float y, float width, float height, float pos)
{
    int domainResolution = 128;
    float xIncr = width/domainResolution;
    juce::Path sinMainPath, offsetLPath, offsetRPath;
    
    float twopi = juce::MathConstants<float>::twoPi;
    sinMainPath.startNewSubPath(x, y + height/2);
    offsetLPath.startNewSubPath(x, y + height/2);
    offsetRPath.startNewSubPath(x, y + height/2);

    float posBipolar = (std::abs(pos - 0.5f) * 2);
    
    for (int i = 1; i < domainResolution; i++)
    {
        int hannCenterPoint = domainResolution * pos;
        int hannWidth = (1.0f - posBipolar) * 64 + 64;
        int hannStartPoint = juce::jlimit(0, domainResolution, hannCenterPoint - hannWidth/2);
        int hannEndPoint = juce::jlimit(0, domainResolution, hannCenterPoint +
                                        hannWidth/2);

        int hannLength = hannEndPoint - hannStartPoint;

        float hann = 0.0f;
        float phase = 0.0f;
        
        if (i > hannStartPoint && i < hannEndPoint){
            int j = i - hannStartPoint;
            hann = 0.5f * (1.0f - std::cos(twopi * j/hannLength));
            
            float frequency = (std::abs(pos - 0.5f) * 2) * 1.0f + 1.0f;
            phase = (twopi * std::pow(frequency, 2) * j) / hannLength;
        }
        
        float sin = std::sin(phase) * hann;
        float sinOffset1 = std::sin(phase * (std::abs(pos - 0.5f) + 0.5f)) * hann;
        float sinOffset2 = std::sin(phase * -(std::abs(pos - 0.5f) + 0.5f)) * hann;

        float heightOffset = (height * 0.25f * (1.0f - posBipolar)) + (height * 0.25f);
        
        juce::Point<float> sinCoords = { x + xIncr * i, (y + height/2) + sin * heightOffset };
        juce::Point<float> offsetLCoords = { sinCoords.x, (y + height/2) + sinOffset1 * heightOffset };
        juce::Point<float> offsetRCoords = { sinCoords.x, (y + height/2) + sinOffset2 * heightOffset };

        
        sinMainPath.lineTo(sinCoords.x, sinCoords.y);
        sinMainPath = sinMainPath.createPathWithRoundedCorners(10);
        
        offsetLPath.lineTo(offsetLCoords.x, offsetLCoords.y);
        offsetLPath = offsetLPath.createPathWithRoundedCorners(10);

        offsetRPath.lineTo(offsetRCoords.x, offsetRCoords.y);
        offsetRPath = offsetRPath.createPathWithRoundedCorners(10);

        g.setColour(juce::Colour(100, 100, 100));
        g.strokePath(offsetLPath, juce::PathStrokeType(1.5));
        
        g.setColour(juce::Colour(150, 150, 150));
        g.strokePath(offsetRPath, juce::PathStrokeType(1.5));
        
        g.setColour(juce::Colour(200, 200, 200));
        g.strokePath(sinMainPath, juce::PathStrokeType(1.5));
    }
}

void SpinnerControlsLookAndFeel::drawRatio(juce::Graphics& g, float x, float y, float size, float index)
{
    juce::Path frontEndPath, frontPath, backPath;
    juce::Point<float> centerCoords = { x + size * 0.5f, y + size * 0.75f };

    int ratioIndex = index * 3.0f + 1.0f;
    ratioIndex = 5 - std::round(ratioIndex);
    
    float startPoint = -1.57f;
    for (int i = 1; i <= 4; i++)
    {
        int j = 5 - i;
        float radius = (size * 0.4f/4) * i;
        float endPoint = startPoint + 3.14f/j;
        
        backPath.addCentredArc(centerCoords.x, centerCoords.y,
                                  radius, radius,
                                  0.0f, startPoint, endPoint, true);
        backPath.lineTo(centerCoords);
        
        g.setColour(juce::Colour(130 - (10 * i), 130 - (10 * i), 130 - (10 * i)));
        
        if (i == ratioIndex)
        {
            float frontRadius = size * 0.1f * ratioIndex;
            float lineAngle = endPoint - 1.57f;

            frontPath.addCentredArc(centerCoords.x, centerCoords.y,
                                    frontRadius, frontRadius,
                                    0.0f, startPoint, endPoint, true);

            juce::Point<float> lineEndCoords = { centerCoords.x + std::cos(lineAngle) * size * 0.5f,
                centerCoords.y + std::sin(lineAngle) * size * 0.5f };

            frontPath.startNewSubPath(centerCoords);
            frontPath.lineTo(lineEndCoords);
            
            frontEndPath.addCentredArc(lineEndCoords.x, lineEndCoords.y, 2.0f, 2.0f, 0.0f, 0.0f, 6.28f);
        }
    }
    
    g.strokePath(backPath, juce::PathStrokeType(1.5f));

    g.setColour(juce::Colour(190, 190, 190));
    g.strokePath(frontPath, juce::PathStrokeType(1.5f));
    g.fillPath(frontEndPath);
}

void SpinnerControlsLookAndFeel::drawOpacity(juce::Graphics& g, float x, float y, float width, float height, float pos)
{
    
    drawCrescent(g,
                 x + width * pos,
                 y + height * 0.25f,
                 width * 0.085f,
                 pos);

    for (int i = 0; i < 8; i++)
    {
        float incr = (width/8) * i;
        drawCrescent(g,
                     x + incr,
                     y + height * 0.75f,
                     width * 0.035f,
                     0.125 * (i + 1));

    }
}

void SpinnerControlsLookAndFeel::drawCrescent(juce::Graphics& g, float x, float y, float size, float amount)
{
    juce::Path outlinePath, fillPath, fillClipPath;
    float radius = size * 0.5f;
    
    // fill
    outlinePath.addCentredArc(x + size/2,
                              y + size/2,
                              radius,
                              radius,
                              0.0f,
                              0.0f,
                              6.28f,
                              true);
    
    juce::Colour fillAdjust = amount < 0.5f ? Colors::backgroundFill : Colors::graphicBlack;
    g.setColour(fillAdjust);
    g.fillPath(outlinePath);

    // clipped
    float radianAdjust = amount >= 0.5f ? 0.0f : 3.14f;
    juce::Colour clipFillAdjust = amount >= 0.5f ? Colors::backgroundFill : Colors::graphicBlack;

    fillPath.addCentredArc(x + size/2,
                           y + size/2,
                           radius,
                           radius,
                           0.0f,
                           0.0f + radianAdjust,
                           3.14f + radianAdjust,
                           true);
    
    float radiusScaled = radius * std::abs((amount - 0.5f) * 2.0f);
    fillPath.addCentredArc(x + size/2,
                           y + size/2,
                           radiusScaled,
                           radius,
                           0.0f,
                           3.14f + radianAdjust,
                           6.28f + radianAdjust,
                           false);
    g.setColour(clipFillAdjust);
    g.fillPath(fillPath);
    
    g.setColour(Colors::backgroundFill);
    g.strokePath(outlinePath, juce::PathStrokeType(1.0f));
}

    SpinnerLayout::SpinnerLayout(TingeAudioProcessor &p, int index) : audioProcessor(p)
    {
        this->index = index;
                
        rateLAF.setIndex(index);
        setSlider(*this, rateFreeSlider, rateLAF);
        juce::String rateFreeID = "rate" + juce::String(index);
        rateFreeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params->apvts, rateFreeID, rateFreeSlider);
        
        setSlider(*this, opacitySlider, opacityLAF);
        opacityLAF.setIndex(index);
        juce::String opacityID = "weight" + juce::String(index);
        opacityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params->apvts, opacityID, opacitySlider);
        
        // rate
        setLabel(*this, rateLabel, "Rate", juce::Justification::centredLeft);
        rateFreeTextSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, "rate" + juce::String(index), "Hz");
        addAndMakeVisible(*rateFreeTextSlider);
        rateFreeTextSlider->setFontColor(Colors::graphicBlack);

        // phase
        setLabel(*this, phaseLabel, "Phase", juce::Justification::centredLeft);
        phaseTextSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, "phase" + juce::String(index), "%");
        addAndMakeVisible(*phaseTextSlider);
        phaseTextSlider->setFontColor(Colors::graphicBlack);

        // opacity
        setLabel(*this, opacityLabel, "Opacity", juce::Justification::centredLeft);
        opacityTextSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, "weight" + juce::String(index), "");
        addAndMakeVisible(*opacityTextSlider);
        opacityTextSlider->setFontColor(Colors::graphicBlack);
    }
    
    SpinnerLayout::~SpinnerLayout()
    {
        rateModeButton.removeListener(this);
    }
    
void SpinnerLayout::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    float margin = bounds.getWidth() * 0.05f;
    bounds.reduce(margin, margin);
    float x = bounds.getX();
    float y = bounds.getY();
    float width = bounds.getWidth();
    float height = bounds.getHeight();

    juce::Path backgroundPath, highlightPath;

    backgroundPath.addRoundedRectangle(x,
                                       y,
                                       width,
                                       height * 0.65f,
                                       margin * 0.5f);
    
    g.setColour(juce::Colour(50, 50, 50));
    g.fillPath(backgroundPath);
    
    highlightPath.startNewSubPath(x, y);
    highlightPath.lineTo(x + width * 0.2f, y);
    highlightPath.lineTo(x + width * 0.075f, y + height * 0.65f);
    highlightPath.lineTo(x, y + height * 0.65f);
    highlightPath.closeSubPath();
    g.setColour(juce::Colour(255, 255, 255).withAlpha((float)0.15f));
    g.fillPath(highlightPath);
    g.reduceClipRegion(backgroundPath);
}

void SpinnerLayout::resized()
{
    auto bounds = getLocalBounds().toFloat();
    float margin = bounds.getWidth() * 0.05f;
    bounds.reduce(margin, margin);
    float x = bounds.getX();
    float y = bounds.getY();
    float width = bounds.getWidth();
    float height = bounds.getHeight();
    
    rateFreeSlider.setBounds(x,
                             y,
                             width,
                             height * 0.65f);
        
    float rowY = y + height * 0.7f;
    float rowHeight = getFontSize() * 1.5f;
    rateLabel.setBounds(x,
                        rowY,
                        width * 0.25f,
                        rowHeight);
//    rateLabel.setFont(juce::FontOptions(getFontSize()));
    
    rateFreeTextSlider->setBounds(x + width * 0.15f,
                                  rowY,
                                  width * 0.25f,
                                  rowHeight);
    rateFreeTextSlider->setFontSize(getFontSize());

    
    phaseLabel.setBounds(x + width * 0.55f,
                         rowY,
                         width * 0.25f,
                         rowHeight);
    
    phaseTextSlider->setBounds(x + width * 0.75f,
                               rowY,
                               width * 0.25f,
                               rowHeight);
    phaseTextSlider->setFontSize(getFontSize());

    
    // opacity
    rowY = y + height * 0.9f;
    opacityLabel.setBounds(x,
                           rowY,
                           width * 0.25f,
                           rowHeight);
    opacityLabel.setFont(juce::FontOptions(getFontSize()));

    opacityTextSlider->setBounds(x + width * 0.25f,
                                 rowY,
                                 width * 0.25f,
                                 rowHeight);
    opacityTextSlider->setFontSize(getFontSize());

}

void SpinnerLayout::buttonClicked(juce::Button* b)
{
        
     if (b == &rateModeButton){
        
        juce::String rateModeID = "rateMode" + juce::String(index);
        bool rateMode = rateModeButton.getToggleState();
        float rateModeFloat = rateMode ? 0.0f : 1.0f;
        audioProcessor.params->apvts.getParameter(rateModeID)->setValueNotifyingHost(rateModeFloat);
        
        if (rateMode)
        {
            rateFreeSlider.setVisible(true);
            rateSyncSlider.setVisible(false);
            rateFreeTextSlider->setVisible(true);
            rateSyncTextSlider->setVisible(false);
            
        } else {
            rateFreeSlider.setVisible(false);
            rateSyncSlider.setVisible(true);
            rateFreeTextSlider->setVisible(false);
            rateSyncTextSlider->setVisible(true);
        }
        
    }
}

bool SpinnerLayout::isMouseOver(juce::Point<int> mouse)
{
    auto bounds = getBounds();
    return bounds.contains(mouse);
}

void SpinnerLayout::timerCallback()
{
    /*
    if (rateFreeTextSlider->isMouseOver()){
        rateFreeSlider.setVisible(true);
        opacitySlider.setVisible(false);

    } else if (opacityTextSlider->isMouseOver())
    {
        rateFreeSlider.setVisible(false);
        opacitySlider.setVisible(true);
    }
*/
}
