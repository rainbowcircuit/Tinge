/*
  ==============================================================================

    InteractionLogic.h
    Created: 7 Jun 2025 3:41:22pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
/*
  ==============================================================================

    Graphics2.h
    Created: 7 Jun 2025 3:18:34pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Interaction // this class is meant for both DSP and Graphics maybe
{
public:
    void setThresholdAngles()
    {
        thresholdAngles[0] = 0.02f;
        thresholdAngles[1] = 0.22f;
        thresholdAngles[2] = 0.35f;
        thresholdAngles[3] = 0.62f;
        thresholdAngles[4] = 0.68f;
        thresholdAngles[5] = 0.79f;
    }

    std::array<float, 16> getRotationAngles(float phase, int division)
    {
        std::array<float, 16> outputAngles = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        if (division < 0) return outputAngles;
        
        int numSegments = division * 2;
        float segmentSize = 1.0f/numSegments;
        
        outputAngles[0] = fmodf(phase, 1.0f);
        for (int i = 1; i < numSegments; i++)
        {
            float angle = fmodf((segmentSize * i + phase), 1.0f);
            outputAngles[i] = angle;
        }

        return outputAngles;
    }
    
    std::array<float, 16> getInteraction(std::array<float, 16> angles, std::array<float, 16> &thresholdAngles, int division)
    {
        std::array<float, 16> thresholdBools;
        for (int i = 0; i < thresholdBools.size(); i++)
        {
            thresholdBools[i] = 0.0f;
        }

        for (int i = 0; i < division * 2 - 1; i++) {
            if (i % 2 == 0) {
                for (int j = 0; j < 6; j++) {
                    if (isOverThreshold(thresholdAngles[j], angles[i], angles[i + 1]))
                        thresholdBools[j] = 1.0f;
                }
            }
        }
        return thresholdBools;
    }
    
    bool isOverThreshold(float value, float start, float end) {
        if (start < end) {
            return value > start && value <= end;
        } else {
            return value > start || value <= end;
        }
    }

    // eventually three of these per rotation
    std::array<float, 32> angles;
    std::array<float, 16> thresholdAngles;
    std::array<float, 16> thresholds;

    
 //   float phase = 0.12f;
 //   int division = 1;

    struct RotationValues
    {
        float phase;
        int division;
        std::array<float, 16> angles; // max 8 divisions
    };
    std::array<RotationValues, 3> rotationValues;
    
private:
    
};

/*
class InteractionGraphics : public juce::Component, Interaction
{
public:
    InteractionGraphics()
    {
        setThresholdAngles();
    }
    
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        float wheelWidth = bounds.getWidth() * 0.55f;
        float x = bounds.getX();
        float y = bounds.getY();
        
        // sum
        drawThreshold(g, x + 100, y + 20, wheelWidth, wheelWidth);
        juce::Colour color1 = {200, 50, 50};
        juce::Colour color2 = {50, 200, 50};
        juce::Colour color3 = {50, 50, 200};
        
       // drawWheel(g, 2, x + 100, y + 40, wheelWidth, wheelWidth, 0.8, color1);
    }
    
    void drawWheel(juce::Graphics& g, int index, int x, int y, int width, int height, float opacity, juce::Colour color)
    {
        float twopi = juce::MathConstants<float>::twoPi;
        float radius = width/2;
        
        int division = 3;
        for(int i = 0; i < division * 2; i+= 2)
        {

            float start = angles[i] * twopi;
            float end = angles[i + 1] * twopi;
    
            juce::Path linePath;
            linePath.startNewSubPath(x + width/2, y + height/2);
            linePath.addCentredArc(x + width/2, y + height/2, radius, radius, 0.0f, start, end);
            linePath.lineTo(x + width/2, y + height/2);

            if (i % 2 == 0){
                g.setColour(color);
                g.fillPath(linePath);
            }
        }
    }
    
    void drawThreshold(juce::Graphics& g, int x, int y, int width, int height)
    {


    }

    void setParams(int index, float phaseValue, int divisionValue, float opacityValue)
    {
        rotationValue[index].phase = phaseValue;
        rotationValue[index].division = divisionValue;
        repaint();
    }
    


    struct RotationValue
    {
        bool state = true;
        float phase = 0.0f;
        float division = 2;
    };
    std::array<RotationValue, 3> rotationValue;
    
private:
    
};
*/
