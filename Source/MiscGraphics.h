/*
  ==============================================================================

    MiscGraphics.h
    Created: 26 Aug 2025 12:31:16pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "LookAndFeel.h"

enum class MiscLAF { Overlap, SpinnerTab, ThresholdTab };
class MiscLookAndFeel : public juce::LookAndFeel_V4, DrawHelper
{
public:
    MiscLookAndFeel(MiscLAF l) : lookAndFeel(l) {}
    
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider) override
    {
        switch(lookAndFeel){
            case MiscLAF::Overlap:
            {
                float position = sliderPosProportional * 6;
                position = std::floor(position);
                drawOverlapSlider(g, x, y, width, position);
                break;
            }
            case MiscLAF::SpinnerTab: { break; }
            case MiscLAF::ThresholdTab: { break; }
        }
    }
    
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        float x = bounds.getX();
        float y = bounds.getY();
        float width = bounds.getWidth();
        float iconSize = bounds.getHeight();

        
        switch(lookAndFeel){
            case MiscLAF::Overlap: { break; }
            case MiscLAF::SpinnerTab:
            {
                drawSpinnerTab(g, (x + width/2) - (iconSize/2), y, iconSize);
                break;
            }
            case MiscLAF::ThresholdTab:
            {
                drawThresholdTab(g, (x + width/2) - (iconSize/2), y, iconSize);
                break;
            }
        }
    }
     
    
    void drawOverlapSlider(juce::Graphics &g, float x, float y, float size, int overlapIndex)
    {
        juce::Path pathA, pathB, pathC;
        float centerX = x + size/2;
        float centerY = y + size/2;
        float offset = size * 0.125f;
        float radius = size * 0.25f;

        pathA.addCentredArc(centerX - offset, centerY + offset, radius, radius, 0.0f, 0.0f, 6.28f, true);
        pathB.addCentredArc(centerX + offset, centerY + offset, radius, radius, 0.0f, 0.0f, 6.28f, true);
        pathC.addCentredArc(centerX, centerY - offset, radius, radius, 0.0f, 0.0f, 6.28f, true);
        
        juce::Colour colorA, colorB, colorC, colorAB, colorBC, colorAC, colorABC;
        
        if (overlapIndex == 0 || overlapIndex == 3 || overlapIndex == 4 || overlapIndex == 6){
            colorA = Colors::primaryColor[0];
            colorB = Colors::primaryColor[1];
            colorC = Colors::primaryColor[2];
        } else {
            colorA = Colors::backgroundFillAlt;
            colorB = Colors::backgroundFillAlt;
            colorC = Colors::backgroundFillAlt;
        }
        
        if (overlapIndex == 1 || overlapIndex == 3 || overlapIndex == 5 || overlapIndex == 6){
            colorAB = Palette::darkenColors(Colors::primaryColor[0], Colors::primaryColor[1]);
            colorBC = Palette::darkenColors(Colors::primaryColor[1], Colors::primaryColor[2]);
            colorAC = Palette::darkenColors(Colors::primaryColor[0], Colors::primaryColor[2]);

        } else {
            colorAB = Colors::backgroundFillAlt;
            colorBC = Colors::backgroundFillAlt;
            colorAC = Colors::backgroundFillAlt;
        }
        
        if (overlapIndex == 2 || overlapIndex == 4 || overlapIndex == 5 || overlapIndex == 6){
            colorABC = Palette::darkenColors(Colors::primaryColor[0], Colors::primaryColor[1], Colors::primaryColor[2]);
        } else {
            colorABC = Colors::backgroundFill;
        }
        
        drawWithoutOverlap(g, pathA, colorA, false);
        drawWithoutOverlap(g, pathB, colorB, false);
        drawWithoutOverlap(g, pathC, colorC, false);
        drawDoubleOverlap(g, pathA, pathB, colorAB, false);
        drawDoubleOverlap(g, pathB, pathC, colorBC, false);
        drawDoubleOverlap(g, pathA, pathC, colorAC, false);
        drawTripleOverlap(g, pathA, pathB, pathC, colorABC, false);
        
    }
    
    void drawSpinnerTab(juce::Graphics& g, float x, float y, float size)
    {
        float radius = size * 0.3f;
        float thirdRadian = juce::MathConstants<float>::twoPi/3;
        
        for(int i = 0; i < 3; i++)
        {
            juce::Path graphicPath;
            graphicPath.addCentredArc(x + size/2,
                                     y + size/2,
                                     radius,
                                     radius,
                                     thirdRadian * i,
                                     0.0f,
                                     thirdRadian,
                                     true);
            graphicPath.lineTo(x + size/2, y + size/2);
            graphicPath.closeSubPath();
            
            g.setColour(Colors::primaryColor[i]);
            g.fillPath(graphicPath);
        }
    }

    void drawThresholdTab(juce::Graphics& g, float x, float y, float size)
    {
        float radius = size * 0.3f;
        float dotSize = size * 0.075f;
        for(int i = 0; i < 8; i++)
        {
            float radian = (juce::MathConstants<float>::twoPi/8) * i;
            juce::Point<float> dotCoords = { (x + size/2) + std::cos(radian) * radius,
                (y + size/2) + std::sin(radian) * radius };
            
            g.setColour(Colors::graphicWhite);
            g.fillEllipse(dotCoords.x - dotSize/2,
                          dotCoords.y - dotSize/2,
                          dotSize,
                          dotSize);
        }
    }
    
    
    
private:
    MiscLAF lookAndFeel;
    Palette p;
};
