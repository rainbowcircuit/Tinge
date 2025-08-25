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
    juce::Path arcPath, highlightPath;
    
    float startAngle = -1.57f;
    float pi = juce::MathConstants<float>::pi;
    float radius = width * 0.45f;
    arcPath.addCentredArc(x + width/2,
                          y + height,
                          radius,
                          radius, 0.0f, startAngle, startAngle + pi, true);
    
    g.setColour(juce::Colour(190, 190, 190));
    g.strokePath(arcPath, juce::PathStrokeType(1.5f));
    
    // highlights
    float highlightAngle = pi/6;
    float highlightStartAngle = startAngle + pos * (pi - highlightAngle);
    float highlightEndAngle = highlightStartAngle + highlightAngle;
    float alpha = pos * 0.65f + 0.2f;
    
    highlightPath.startNewSubPath(x + width/2, y + height);
    highlightPath.addCentredArc(x + width/2,
                                y + height,
                                radius * 0.85f,
                                radius * 0.85f,
                                0.0f,
                                highlightStartAngle,
                                highlightEndAngle);
    highlightPath.closeSubPath();
    g.setColour(Colors::primaryColor[spinnerIndex].withAlpha(float(alpha)));
    g.fillPath(highlightPath);

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
        
        // clean this up later
        setLabel(*this, rateLabel, "Rate", juce::Justification::centredLeft);
        setLabel(*this, phaseLabel, "Phase", juce::Justification::centredLeft);
        setLabel(*this, opacityLabel, "Opacity", juce::Justification::centredLeft);

        rateFreeTextSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, "rate" + juce::String(index), "Hz");
        addAndMakeVisible(*rateFreeTextSlider);
        rateFreeTextSlider->setFontSize(12.0f);
        
        phaseTextSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, "phase" + juce::String(index), "%");
        addAndMakeVisible(*phaseTextSlider);
        phaseTextSlider->setFontSize(12.0f);

        opacityTextSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, "weight" + juce::String(index), "");
        addAndMakeVisible(*opacityTextSlider);
        opacityTextSlider->setFontSize(12.0f);
        
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
                                       height * 0.5f,
                                       margin * 0.5f);
    
    g.setColour(juce::Colour(50, 50, 50));
    g.fillPath(backgroundPath);
    
    highlightPath.startNewSubPath(x, y);
    highlightPath.lineTo(x + width * 0.2f, y);
    highlightPath.lineTo(x + width * 0.075f, y + height * 0.5f);
    highlightPath.lineTo(x, y + height * 0.5f);
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
    
    rateFreeSlider.setBounds(x, y, width, height * 0.5f);
    rateLabel.setBounds(x, height * 0.85f, width, height * 0.15f);

    rateFreeTextSlider->setBounds(x + width * 0.15f, y + height * 0.85f, width * 0.25f, height * 0.15f);
    
    phaseLabel.setBounds(x + width * 0.55f, height * 0.85f, width * 0.25f, height * 0.15f);
    phaseTextSlider->setBounds(x + width * 0.75f, height * 0.85f, width * 0.25f, height * 0.15f);
    
    opacityTextSlider->setBounds(x + width * 0.15f, y + height * 0.85f, width * 0.25f, height * 0.15f);

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

