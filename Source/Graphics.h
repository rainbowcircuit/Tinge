
#pragma once
#include "InteractionLogic.h"
#include "LookAndFeel.h"
#include "DSP.h"
#include "GraphicsHelper.h"

class SpinnerGraphics : public juce::Component, public Interaction, public DrawHelper, public juce::Timer
{
public:
    SpinnerGraphics();
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    //  ==============================================================================
    void setWheelPosition();
    juce::Path generateWheelPath(juce::Graphics& g, int index, bool isSum);
    void drawThreshold(juce::Graphics& g, int x, int y, int width, int height);
    void setThresholdSlew(float slewTime);
    //==============================================================================

    void setOverlapIndex(int overlapIndex);
    void setOverlapColours();
    //  ==============================================================================

    void setParams(int index, int ratio, int colorIndex, float opacity);
    void setPhase(int index, float phaseValue);
    void setAnimation(float value);
    void setAnimationValue(float animationValue);
    
    

private:
    void timerCallback() override;

    LowPassGate thresholdSlew;
    
    juce::Colour colorA, colorB, colorC, colorAB, colorBC, colorAC, colorABC;
    
    int overlapIndex;
    float animationValue;
    float isometricSkew;
    float isometricOffset;
    juce::Point<float> dragStartPoint;
    float yOffset, yPosOffset;
    Palette p;
};

/*
class MeterGraphics : public juce::Component, public Interaction, public DrawHelper, public juce::Timer
{
public:
    MeterGraphics()
    {
        startTimerHz(30);
    }
    
    void paint(juce::Graphics& g) override
    {
        auto fillBounds = bounds;
        g.setColour(Colors::backgroundFillAlt);
        g.fillRoundedRectangle(fillBounds, 3);
        drawMeter(g);
    }
    
    void resized() override
    {
        bounds = getLocalBounds().toFloat();
    }
    
    void drawMeter(juce::Graphics &g)
    {
        float graphicWidth = bounds.getWidth() * 0.9f;
        float widthMargin = bounds.getWidth() * 0.05f;
        float widthIncr = graphicWidth/128;
        
        float graphicHeight = bounds.getHeight() * 0.9f;
        for (int i = 0; i < numThresholds; i++)
        {
            juce::Path meterPath;
            
            meterPath.startNewSubPath(widthMargin, bounds.getCentreY());
            for (int j = 0; j < 128; j++)
            {
                float meterValue = juce::jlimit(0.0f, 1.0f, measurement[i][j]);
                meterPath.lineTo(bounds.getX() + widthIncr * j,
                                 bounds.getCentreY() + meterValue * 20.0f);
                
                meterPath = meterPath.createPathWithRoundedCorners(2);
            }
            g.setColour(juce::Colour(0, 0, 0));
            g.strokePath(meterPath, juce::PathStrokeType(2.0f));
        }
    }
    
    void setMeterArray()
    {
        meterWriteIndex += 1;
        if (meterWriteIndex > 127) { meterWriteIndex = 0; }
        
        for (int i = 0; i < numThresholds; i++){
            float weightA = ((float)rotationValue[0].threshold[i] * rotationValue[0].opacity)/3.0f;
            float weightB = ((float)rotationValue[1].threshold[i] * rotationValue[1].opacity)/3.0f;
            float weightC = ((float)rotationValue[2].threshold[i] * rotationValue[2].opacity)/3.0f;
            float thresholdWeight = (weightA + weightB + weightC);
            measurement[i][meterWriteIndex] = thresholdWeight;
        }
    }
    
    void setPhase(int index, float phaseValue)
    {
        rotationValue[index].phase = phaseValue;
    }

    
    void timerCallback() override
    {
        setMeterArray();
        repaint();
    }
private:
    juce::Rectangle<float> bounds;
    Palette p;
    int meterWriteIndex = 0;
    std::array<std::array<float, 128>, 16> measurement;
};
*/
