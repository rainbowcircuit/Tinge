
#pragma once
#include "InteractionLogic.h"

class SpinnerGraphics : public juce::Component, Interaction
{
public:
    SpinnerGraphics()
    {
        rotationValue[0].state = true;
        rotationValue[1].state = true;
        rotationValue[2].state = true;
    }
    
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        float wheelWidth = bounds.getWidth() * 0.55f;
        float x = bounds.getX();
        float y = bounds.getY();
        
        // sum
        juce::Colour color0 = { 255, 0, 0 };
        juce::Colour color1 = { 0, 0, 255 };
        juce::Colour color2 = { 0, 255, 0 };

        std::array<float, 16> thresAngles = { 0.16, 0.33, 0.5, 0.66, 0.83, 1.0 };
        std::array<float, 16> myAngles0 = getRotationAngles(rotationValue[0].phase, rotationValue[0].division);
        std::array<float, 16> myAngles1 = getRotationAngles(rotationValue[1].phase, rotationValue[1].division);
        std::array<float, 16> myAngles2 = getRotationAngles(rotationValue[2].phase, rotationValue[2].division);

        std::array<float, 16> thresBool0 = getInteraction(myAngles0, thresAngles, rotationValue[0].division);
        std::array<float, 16> thresBool1 = getInteraction(myAngles1, thresAngles, rotationValue[1].division);
        std::array<float, 16> thresBool2 = getInteraction(myAngles2, thresAngles, rotationValue[2].division);
        
        
        drawWheel(g, myAngles2, rotationValue[2].phase, rotationValue[2].division, true, x + 20, y + 165, wheelWidth, wheelWidth, rotationValue[2].opacity, color2);
        drawWheel(g, myAngles1, rotationValue[1].phase, rotationValue[1].division, true, x + 20, y + 165, wheelWidth, wheelWidth, rotationValue[1].opacity, color1);
        drawWheel(g, myAngles0, rotationValue[0].phase, rotationValue[0].division, true, x + 20, y + 165, wheelWidth, wheelWidth, rotationValue[0].opacity, color0);
        drawThreshold(g, thresAngles, thresBool0, thresBool1, thresBool2,
                      rotationValue[0].state, rotationValue[1].state, rotationValue[2].state,
                      x + 20, y + 110, wheelWidth, wheelWidth);

        // actual wheels
        drawWheel(g, myAngles2, rotationValue[2].phase, rotationValue[2].division, true, x + 20, y + 80, wheelWidth, wheelWidth, rotationValue[2].opacity, color2);
        drawWheel(g, myAngles1, rotationValue[1].phase, rotationValue[1].division, true, x + 20, y + 70, wheelWidth, wheelWidth, rotationValue[1].opacity, color1);
        drawWheel(g, myAngles0, rotationValue[0].phase, rotationValue[0].division, true, x + 20, y + 60, wheelWidth, wheelWidth, rotationValue[0].opacity, color0);
    }
    
    void drawWheel(juce::Graphics& g, std::array<float, 16> angles, float phasePosition, int division, bool state, int x, int y, int width, int height, float opacity, juce::Colour color)
    {
        /*
        DBG(angles[0] << " 0");
        DBG(angles[1] << " 1");
        DBG(angles[2] << " 2");
        DBG(angles[3] << " 3");
        DBG(angles[4] << " 4");
        DBG(angles[5] << " 5");
        DBG(angles[6] << " 6");
        DBG(angles[7] << " 7");
        DBG(angles[8] << " 8");
        DBG(angles[9] << " 9");
        DBG(angles[10] << " 10");
        DBG(angles[11] << " 11");
        DBG(angles[12] << " 12");
        DBG(angles[13] << " 13");
        DBG(angles[14] << " 14");
        DBG(angles[15] << " 15");
        */
        
        if (division < 0) { division = 1; }
        
        float twopi = juce::MathConstants<float>::twoPi;
        float radius = width/2;
        
        for(int i = 0; i < division * 2 - 1; i+= 2){
            
            float startAngle = angles[i] * twopi;
            float endAngle = angles[i + 1] * twopi;

            if (endAngle <= startAngle)
                endAngle += twopi;
            
            juce::Path wheelPath;
            wheelPath.startNewSubPath(x + width/2, y + height/4); // center
            wheelPath.addCentredArc(x + width/2, y + height/4, radius, radius/2, 0.0f, startAngle, endAngle);
            wheelPath.closeSubPath();
            wheelPath = wheelPath.createPathWithRoundedCorners(4);

            if (i % 2 == 0)
            {
                g.setColour(color.withAlpha(opacity));
                g.fillPath(wheelPath);
                
            } else if (i % 2 == 1){
                g.setColour(juce::Colour(250, 250, 250).withAlpha(0.0f));
                g.fillPath(wheelPath);
            }
        }
    }
        
    void drawThreshold(juce::Graphics& g, std::array<float, 16> angles, std::array<float, 16> thresBool0, std::array<float, 16> thresBool1, std::array<float, 16> thresBool2, bool state0, bool state1, bool state2, int x, int y, int width, int height)
    {
        float twopi = juce::MathConstants<float>::pi * 2;
        float radius = width * 0.45f;
        int numNotes = 6;
        for (int i = 0; i < numNotes; i++)
        {
            float startCoords = (x + width/2) + std::cos(angles[i] * twopi) * radius;
            float endCoords = (y + height/2) + std::sin(angles[i] * twopi) * radius/2;

            juce::Path graphicPath;
            graphicPath.startNewSubPath(x + width/2, y + height/2);
            graphicPath.lineTo(startCoords, endCoords);
            g.setColour(juce::Colour(40, 40, 40));
            
            
            bool bool0 = thresBool0[i] > 0.0f && state0;
            bool bool1 = thresBool1[i] > 0.0f && state1;
            bool bool2 = thresBool2[i] > 0.0f && state2;

            if (bool0 || bool1 || bool2){
            
                juce::Colour myColour = juce::Colour(thresBool0[i] * 250, thresBool1[i]  * 250 , thresBool2[i] * 250);
                float alpha = (thresBool0[i] + thresBool1[i] + thresBool2[i])/3.0f;
                g.setColour(myColour);
            }
            g.strokePath(graphicPath, juce::PathStrokeType(2));
        }
        
        // outer rings
        
    }
    
    
    
    
    
    
    void resized() override {}
    
    void setParams(int index, float phaseValue, int divisionValue, bool stateValue, float opacityValue)
    {
        rotationValue[index].phase = phaseValue;
        rotationValue[index].division = divisionValue;
        rotationValue[index].state = stateValue;
        rotationValue[index].opacity = opacityValue;

        repaint();
    }
    


private:
    struct RotationValue
    {
        bool state = true;
        float phase = 0.0f;
        float division;
        float opacity;
    };

    std::array<RotationValue, 3> rotationValue;

    
    
    struct Threshold
    {
        float anglePosition;
        bool threshold;
    };
    
    std::array<Threshold, 16> thresholdValue;
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
