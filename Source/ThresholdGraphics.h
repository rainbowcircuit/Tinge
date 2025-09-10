/*
  ==============================================================================

    ThresholdGraphics.h
    Created: 9 Sep 2025 9:59:14am
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "InteractionLogic.h"
#include "LookAndFeel.h"

class ThresholdGraphics : public juce::Component, Interaction
{
public:
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        g.setColour(Colors::graphicGrey.withAlpha((float)0.05f));
        auto fillBounds = bounds;
        fillBounds.reduce(bounds.getWidth() * 0.025f, bounds.getWidth() * 0.025f);
        g.fillRoundedRectangle(fillBounds, fillBounds.getWidth() * 0.05f);
        
        float radiusIncr = (bounds.getWidth() * 0.4f)/maxThreshold;
        float centerOffsetRadius = bounds.getWidth() * 0.35f;
        juce::Point<float> centerCoords = { bounds.getCentreX(), bounds.getCentreY() };

        float pi = juce::MathConstants<float>::pi;
        float twopi = pi * 2.0f;
        
        juce::Path arcPath, arcRemainderPath, arcEndPath;

        for (int i = 1; i <= maxThreshold; i++)
        {
            int j = maxThreshold - i;
            auto angles = thresholdAngles[i - 1] * twopi;
            angles = std::fmodf(angles, twopi);
            
            float radius = radiusIncr * i;
            float yOffset = ((0.5f - startPhase/twopi) * centerOffsetRadius) / maxThreshold;
            yOffset = yOffset * j;

            float startAngle = std::fmodf(startPhase, twopi);
            float endAngle = std::fmodf(angles, twopi);
            if (endAngle < startAngle) {
                endAngle += twopi;
            }
            
            arcRemainderPath.addCentredArc(centerCoords.x,
                                           centerCoords.y + yOffset,
                                           radius,
                                           radius,
                                           0.0f,
                                           endAngle,
                                           startAngle + twopi,
                                           true);
            
            arcPath.addCentredArc(centerCoords.x,
                                      centerCoords.y + yOffset,
                                      radius,
                                      radius,
                                      0.0f,
                                      startAngle,
                                      endAngle,
                                      true);
            
            
            float arcEndAngle = endAngle - pi/2;
            arcEndPath.addCentredArc(centerCoords.x + std::cos(arcEndAngle) * radius,
                                     centerCoords.y +  + yOffset + std::sin(arcEndAngle) * radius,
                                     2,
                                     2,
                                     0.0f,
                                     0.0f,
                                     6.28f,
                                     true);

        }
        
        g.setColour(Colors::graphicGrey);
        g.strokePath(arcPath, juce::PathStrokeType(1.0f));
        g.fillPath(arcEndPath);

        g.setColour(Colors::graphicGrey.withAlpha((float)0.25f));
        g.strokePath(arcRemainderPath, juce::PathStrokeType(1.0f));
    }
    
    void resized() override {}
    
    void setParams(int mode, float startPhase, int maxThreshold)
    {
        float twopi = juce::MathConstants<float>::twoPi;
        this->startPhase = (startPhase/100.0f) * twopi;
        float originalStartPhase = startPhase;
        
        this->maxThreshold = maxThreshold;
        
        Interaction::numThresholds = 16;
        processThreshold(static_cast<MIDIProcessor::thresholdMode>(mode),
                         originalStartPhase,
                         maxThreshold);
        repaint();
    }
    
private:
    float startPhase = 3.14f;
    int maxThreshold = 15;
};
