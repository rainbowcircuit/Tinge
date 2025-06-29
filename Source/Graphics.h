
#pragma once
#include "InteractionLogic.h"

class SpinnerGraphics : public juce::Component, public Interaction
{
public:
    SpinnerGraphics();
    void paint(juce::Graphics& g) override;
    
    void drawWheel(juce::Graphics& g, int index, int x, int y, int width, int height);
    void drawSummedWheel(juce::Graphics& g, int x, int y, int width, int height);
    void drawThreshold(juce::Graphics& g, int x, int y, int width, int height);
    juce::Path wheelPath(juce::Graphics& g, int index, int x, int y, int width, int height);
    
    void drawDoubleOverlap(juce::Graphics& g, juce::Path& pathA, juce::Path& pathB, juce::Colour color);
    void drawTripleOverlap(juce::Graphics& g, juce::Path& pathA, juce::Path& pathB, juce::Path& pathC, juce::Colour color);
    

    
    
    void sumAngles();
    void resized() override {}
    
    void setParams(int index, float phaseValue, int divisionValue, bool stateValue, float opacityValue);
//    void setNumThresholds(std::array<float, 16> heldNotes);

    void mouseDown(const juce::MouseEvent& event) override
    {
            if (isoView){
                isoEnterToggle.start();
                isoView = false;
            } else {
                isoExitToggle.start();
                isoView = true;
            }
        
    }
    
private:

    float isometricSkew = 2.0f;
    float yPosOffset = 0.0f;
    bool isoViewValue = 0.0f;
    bool isoView = false;
    bool isoAlpha = 1.0f;
    juce::VBlankAnimatorUpdater updater { this };
    
    juce::Animator isoEnterToggle = juce::ValueAnimatorBuilder {}
        .withEasing(juce::Easings::createEaseInOut())
        .withDurationMs(1800.0f)
        .withValueChangedCallback([this] (auto value){ // lambda function
            isometricSkew = value + 1.0f;
            yPosOffset = 1.0f - value;
            repaint();
        })
        .build();
    
    juce::Animator isoExitToggle = juce::ValueAnimatorBuilder {}
        .withEasing(juce::Easings::createEaseInOut())
        .withDurationMs(1800.0f)
        .withValueChangedCallback([this] (auto value){ // lambda function
            isometricSkew = (1.0f - value) + 1.0f;
            yPosOffset = value;
            repaint();
        })
        .build();
    
    juce::Colour color0 = { 255, 0, 0 };
    juce::Colour color1 = { 0, 0, 255 };
    juce::Colour color2 = { 0, 255, 0 };
};


class DialGraphics : public juce::LookAndFeel_V4
{
public:
    DialGraphics(int index)
    {
        this->index = index;
    }
    
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider) override
    {
        auto bounds = slider.getLocalBounds().toFloat();
        float xPos = bounds.getX();
        float yPos = bounds.getY();
        float size = bounds.getWidth();
        
        switch(index)
        {
            case 0:
                drawRateSlider(g, xPos, yPos, size, sliderPosProportional);
                break;
            case 1:
                drawDivisionSlider(g, xPos, yPos, size, sliderPosProportional);
                break;
        }
    }
    
    void drawRateSlider(juce::Graphics &g, float x, float y, float size, float position)
    {
        float width = size * 0.8f;
        float widthMargin = size * 0.1f;
        float posScaled = std::abs(position - 0.5f);
        float needleWidth = width * 0.05f + (width * 0.35f * posScaled);

        
        float height = size * 0.5f;
        float heightMargin = size * 0.25f;
        float baseHeight = height * 0.1f;
        float needleHeight = height * 0.9f;
        
        juce::Point<float> topLeftCoords = { x + widthMargin, y + heightMargin + height - baseHeight };
        juce::Point<float> botLeftCoords = { x + widthMargin, y + heightMargin + height };
        juce::Point<float> topRightCoords = { x + widthMargin + width, y + heightMargin + height - baseHeight };
        juce::Point<float> botRightCoords = { x + widthMargin + width, y + heightMargin + height };

        float needlePos = (needleWidth/2) + width * 0.6f * position;
        juce::Point<float> midCoords = { x + widthMargin + needlePos, y + heightMargin };
        juce::Point<float> midLeftCoords = { midCoords.x - needleWidth/2, topLeftCoords.y};
        juce::Point<float> midRightCoords =  { midCoords.x + needleWidth/2, topRightCoords.y};
        
        juce::Path graphicPath;
        graphicPath.startNewSubPath(topLeftCoords);
        graphicPath.lineTo(midLeftCoords);
        graphicPath.lineTo(midCoords);
        graphicPath.lineTo(midRightCoords);
        graphicPath.lineTo(topRightCoords);
        graphicPath = graphicPath.createPathWithRoundedCorners(10);

        graphicPath.startNewSubPath(botRightCoords);
        graphicPath.lineTo(botLeftCoords);

        g.setColour(juce::Colour(50, 50, 50));
        g.strokePath(graphicPath, juce::PathStrokeType(2.0f));

    }
    
    void drawDivisionSlider(juce::Graphics &g, float x, float y, float size, float position)
    {
        float sliderPos = position * 7.0f;
        sliderPos = std::floor(sliderPos) + 1;
        
        float radius = size * 0.35f;
        float twopi = juce::MathConstants<float>::twoPi;
                
        float segmentSize = twopi/sliderPos;
        float phaseOffset = position * twopi;
        for (int i = 0; i < sliderPos; i++)
        {
            float startAngle = phaseOffset + i * segmentSize;
            float endAngle = phaseOffset + (i + 1) * segmentSize;
            
            if (i % 2 == 0) {
            juce::Path graphicPath;
                float heightOffset = i * 2;
                graphicPath.startNewSubPath(x + size/2, (y + size/2) + heightOffset);
                graphicPath.addCentredArc(x + size/2, (y + size/2) + heightOffset, radius, radius/2, 0.0f, startAngle, endAngle);
                graphicPath.closeSubPath();

                g.setColour(juce::Colour(50, 50, 50).withAlpha((0.5f/(i + 1) + 0.5f)));
                g.fillPath(graphicPath);
            }
        }
    }

private:
    int index;
};
