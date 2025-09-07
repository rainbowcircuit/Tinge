
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
    juce::Path generateWheelPath(juce::Graphics& g, int index);
    juce::Path generateSegmentedWheelPath(juce::Graphics& g, int startIndex, int endIndex);
    void drawThreshold(juce::Graphics& g);
    void setThresholdSlew(float slewTime);
    //==============================================================================

    void setOverlapIndex(int overlapIndex);
    void setOverlapColours();
    //  ==============================================================================

    void setParams(int index, float phase, float opacity1, float opacity2, float opacity3);
    void setAnimation(float value);
    void setAnimationValue(float animationValue);
    
    std::array<float, 2> clipValues(float startA, float endA, float startB, float endB, float startC, float endC)
    {
        std::array<float,2> clippedAB = clipValues(startA, endA, startB, endB);
        float s = clippedAB[0];
        float e = clippedAB[1];
        if (s > e)
            e -= 1.0f;

        std::array<float,2> clippedABC = clipValues(clippedAB[0], clippedAB[1], startC, endC);
        return clippedABC;

    }
    
    /*
    std::array<float, 2> clipValues(float startA, float endA, float startB, float endB)
    {
        std::array<float, 2> clipped = { 0.0f, 0.5f };
        
        bool wrapsA = startA > endA;
        bool wrapsB = startB > endB;

        if (!wrapsA && !wrapsB)
        {
            float s = std::max(startA, startB);
            float e = std::min(endA, endB);
            if (s < e)
                clipped = { s, e };
            
        } else if (wrapsA && !wrapsB)
        {
            float s1 = std::max(startA, startB);
            float e1 = std::min(1.0f,   endB);
            if (s1 < e1)
                clipped = { s1, e1 };

            float s2 = std::max(0.0f, startB);
            float e2 = std::min(endA, endB);
            if (s2 < e2)
                clipped = { s2, e2 };
            
        } else if (!wrapsA && wrapsB) {
            float s1 = std::max(startA, startB);
            float e1 = std::min(endA,   1.0f);
            if (s1 < e1)
                clipped = { s1, e1 };

            float s2 = std::max(startA, 0.0f);
            float e2 = std::min(endA,   endB);
            if (s2 < e2)
                clipped = { s2, e2 };
            
        } else {
            float s = std::max(startA, startB);
            float e = std::min(endA, endB);
            if (s > e)
                e += 1.0f;
            
            clipped = { s, e };
            
        }
        return clipped;
    }
    */
    
    std::array<float, 2> clipValues(float startA, float endA, float startB, float endB)
    {
        std::array<float, 2> clipped = { 0.0f, 0.0f };

        auto normalize = [](float v) {
            if (v < 0.0f) return v + 1.0f;
            if (v >= 1.0f) return v - 1.0f;
            return v;
        };

        // Handle wrap-around by mapping everything to [0,2) range
        float sA = startA;
        float eA = (endA < startA) ? endA + 1.0f : endA;
        float sB = startB;
        float eB = (endB < startB) ? endB + 1.0f : endB;

        // Intersection
        float s = std::max(sA, sB);
        float e = std::min(eA, eB);

        if (s < e) {
            clipped[0] = normalize(s);
            clipped[1] = normalize(e);
        } else {
            // No overlap, return empty interval
            clipped[0] = clipped[1] = 0.0f;
        }

        return clipped;
    }

    
    juce::Path createWheelPath(juce::Graphics &g, juce::Rectangle<float> bounds, float startAngle, float endAngle)
    {
        
        float x = bounds.getX();
        float y = bounds.getY();
        float width = bounds.getWidth();
        float height = bounds.getHeight();

        juce::Path wheelPath;
        float radius = width/2;
        float twopi = juce::MathConstants<float>::twoPi;

        wheelPath.startNewSubPath(x + width/2, y + height/(isometricSkew)); // center
        wheelPath.addCentredArc(x + width/2,
                                y + height/(isometricSkew),
                                radius,
                                radius/(isometricSkew),
                                0.0f,
                                startAngle * twopi,
                                endAngle * twopi);
        
        wheelPath.closeSubPath();
        wheelPath = wheelPath.createPathWithRoundedCorners(1);

    return wheelPath;
    }
    

private:
    void timerCallback() override;

    LowPassGate thresholdSlew;
    juce::Rectangle<float> sumBounds;
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
