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

enum class MiscLAF { Overlap };
class MiscLookAndFeel : public juce::LookAndFeel_V4, DrawHelper
{
public:
    MiscLookAndFeel(MiscLAF l) : lookAndFeel(l)
    {

    }
    
    void setColor(float opacityA, float opacityB, float opacityC)
    {
        p.setColors(opacityA, opacityB, opacityC);
    }
    
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider) override
    {
        auto bounds = slider.getLocalBounds().toFloat();
        
        switch(lookAndFeel){
            case MiscLAF::Overlap:
            {
                float position = sliderPosProportional * 6;
                position = std::floor(position);
                drawOverlapSlider(g, x, y, width, position);
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
        
    
private:
    MiscLAF lookAndFeel;
    Palette p;
};
