
#pragma once
#include "InteractionLogic.h"
#include "LookAndFeel.h"

class DrawHelper
{
public:
    void drawWithoutOverlap(juce::Graphics& g, juce::Path& path, juce::Colour color)
    {
        g.setColour(color);
        g.fillPath(path);
    }

    void drawDoubleOverlap(juce::Graphics& g, juce::Path& pathA, juce::Path& pathB, juce::Colour color)
    {
        g.saveState();
    //    juce::AffineTransform yOffset = juce::AffineTransform::translation(0.0f, -7.0f);
    //    pathA.applyTransform(yOffset);
     //   pathB.applyTransform(yOffset);
        g.reduceClipRegion(pathA);
        g.reduceClipRegion(pathB);
        g.setColour(color);
        g.fillAll();
        g.restoreState();
    }

    void drawTripleOverlap(juce::Graphics& g, juce::Path& pathA, juce::Path& pathB, juce::Path& pathC, juce::Colour color)
    {
        g.saveState();
    //    juce::AffineTransform yOffset = juce::AffineTransform::translation(0.0f, -14.0f);
    //    pathA.applyTransform(yOffset);
    //    pathB.applyTransform(yOffset);
    //    pathC.applyTransform(yOffset);
        g.reduceClipRegion(pathA);
        g.reduceClipRegion(pathB);
        g.reduceClipRegion(pathC);

        g.setColour(color);
        g.fillAll();
        g.restoreState();
    }
};

class WheelGraphics : public juce::Component, public Interaction
{
public:
    void setWheelPosition(int index, int x, int y, int width, int height)
    {
        this->index = index;
        rotationValue[index].bounds.setBounds(x, y, width, height);
    }
        
    void setAnimationValue(float animationValue)
    {
        this->animationValue = animationValue;
    }
    
    juce::Path generateWheelPath(juce::Graphics& g)
    {
        float x = rotationValue[index].bounds.getX();
        float y = rotationValue[index].bounds.getY();
        float width = rotationValue[index].bounds.getWidth();
        float height = rotationValue[index].bounds.getHeight();
        
        int ratio = rotationValue[index].ratio;
        if (ratio < 0) { ratio = 1; }
        
        float twopi = juce::MathConstants<float>::twoPi;
        float radius = width/2;
        
        juce::Path wheelPath;
        for(int i = 0; i < ratio * 2 - 1; i+= 2){
            
            float startAngle = rotationValue[index].angles[i] * twopi;
            float endAngle = rotationValue[index].angles[i + 1] * twopi;

            if (endAngle <= startAngle)
                endAngle += twopi;
            if (i % 2 == 0){
                
                wheelPath.startNewSubPath(x + width/2, y + height/(isometricSkew)); // center
                wheelPath.addCentredArc(x + width/2, y + height/(isometricSkew),
                                        radius, radius/(isometricSkew),
                                        0.0f, startAngle, endAngle);
                wheelPath.closeSubPath();
                wheelPath = wheelPath.createPathWithRoundedCorners(1);
            }
        }
        return wheelPath;
    }

    //************ Mouse Interaction ************//
    
    bool isMouseOver(const juce::MouseEvent& m)
    {
        auto mouse = m.getPosition().toFloat();
        mouseOver = wheelPath.contains(mouse);
        return mouseOver;
    }
    
    void setInFocus(bool inFocus)
    {
        this->inFocus = inFocus;
    }
    
    bool getInFocus()
    {
        return inFocus;
    }


private:
    int index;
    juce::Path wheelPath;
    bool mouseOver, inFocus;
    
    float animationValue;
    float isometricSkew;
    float yOffset, yPosOffset;
};


class SpinnerGraphics : public juce::Component, public Interaction, public DrawHelper
{
public:
    SpinnerGraphics()
    {
        
    }
    
                   
    void paint(juce::Graphics& g) override
    {
        setWheelPosition();


        // summed wheel
        auto sumA = generateWheelPath(g, 2, true);
        auto sumB = generateWheelPath(g, 1, true);
        auto sumC = generateWheelPath(g, 0, true);

        drawWithoutOverlap(g, sumC, colorC);
        drawWithoutOverlap(g, sumB, colorB);
        drawWithoutOverlap(g, sumA, colorA);
        drawDoubleOverlap(g, sumA, sumB, colorAB);
        drawDoubleOverlap(g, sumB, sumC, colorBC);
        drawDoubleOverlap(g, sumA, sumC, colorAC);
        drawTripleOverlap(g, sumA, sumB, sumC, colorABC);
        
        
        // remove this later
        auto bounds = getLocalBounds().toFloat();
        float wheelWidth = bounds.getWidth() * 0.75f;
        float wheelMargin = bounds.getWidth() * 0.125f;
        float x = bounds.getX();
        float y = bounds.getY();
        float sumYScale = animationValue * bounds.getHeight() * 0.25f;
        float sumYOffset = (1.0f - animationValue) * bounds.getHeight() * 0.45f;
        
        drawThreshold(g, x + wheelMargin, y - sumYScale + sumYOffset, wheelWidth, wheelWidth);
        
        
        // single wheel
        auto C = generateWheelPath(g, 2, false);
        auto B = generateWheelPath(g, 1, false);
        auto A = generateWheelPath(g, 0, false);

        drawWithoutOverlap(g, A, p.colorA);
        drawWithoutOverlap(g, B, p.colorB);
        drawWithoutOverlap(g, C, p.colorC);
    }
    
    void setOverlapIndex(int overlapIndex)
    {
        this->overlapIndex = overlapIndex;
        setOverlapColours();
    }
    
    void setOverlapColours()
    {
        if (overlapIndex == 0 || overlapIndex == 3 || overlapIndex == 4 || overlapIndex == 6){
            colorA = p.colorA;
            colorB = p.colorB;
            colorC = p.colorC;
        } else {
            colorA = Colors::backgroundFill;
            colorB = Colors::backgroundFill;
            colorC = Colors::backgroundFill;
        }
        
        if (overlapIndex == 1 || overlapIndex == 3 || overlapIndex == 5 || overlapIndex == 6){
            colorAB = p.colorAB;
            colorBC = p.colorBC;
            colorAC = p.colorAC;
        } else {
            colorAB = Colors::backgroundFill;
            colorBC = Colors::backgroundFill;
            colorAC = Colors::backgroundFill;
        }
        
        if (overlapIndex == 2 || overlapIndex == 4 || overlapIndex == 5 || overlapIndex == 6){
            colorABC = p.colorABC;
        } else {
            colorABC = Colors::backgroundFill;
        }
    }
    
    
    
    void drawThreshold(juce::Graphics& g, int x, int y, int width, int height)
    {
        float twopi = juce::MathConstants<float>::twoPi;
        float pi = juce::MathConstants<float>::pi;

        float innerRadius = width * 0.5f;
        float outerRadius = width * 0.55f;
        
        for (int i = 0; i < numThresholds; i++)
        {
            if (numThresholds > 0 && numThresholds <= 16){
                float angleOffset = fmodf(thresholdAngles[i] - pi - 0.1, 1.0f);
                float cos = std::cos(angleOffset * twopi);
                float sin = std::sin(angleOffset * twopi);

                juce::Point thresholdStart = { (x + width/2) + cos * innerRadius,
                    (y + height/isometricSkew) + sin * innerRadius/isometricSkew };
                
                juce::Point thresholdEnd = { (x + width/2) + cos * outerRadius,
                    (y + height/isometricSkew) + sin * outerRadius/isometricSkew };

                juce::Path graphicPath;
                graphicPath.startNewSubPath(thresholdStart.x, thresholdStart.y);
                graphicPath.lineTo(thresholdEnd.x, thresholdEnd.y);
                graphicPath.lineTo(thresholdEnd.x, thresholdEnd.y - 10.0f);
                graphicPath.lineTo(thresholdStart.x, thresholdStart.y - 10.0f);

                bool triggerCondition = getTriggerCondition(i, overlapIndex);
                
                float alpha = triggerCondition ? 0.875f : 0.25f;
                g.setColour(Colors::graphicWhite.withAlpha((float)alpha));
                g.fillPath(graphicPath);
                g.strokePath(graphicPath, juce::PathStrokeType(2.0f));
            }
        }
    }

    void setAnimation(float value)
    {
        animationValue = value;
        isometricSkew = (1.0f - animationValue) + 1.0f;
    }
    
    void resized() override
    {
        setWheelPosition();
    }
    
    void setParams(int index, int ratio, bool state, int colorIndex, float opacity)
    {
        rotationValue[index].ratio = ratio;
        rotationValue[index].state = state;
        rotationValue[index].colorIndex = colorIndex;
        rotationValue[index].opacity = opacity;
        
        p.setColors(rotationValue[0].colorIndex,
                    rotationValue[1].colorIndex,
                    rotationValue[2].colorIndex,
                    rotationValue[0].opacity,
                    rotationValue[1].opacity,
                    rotationValue[2].opacity);

        repaint();
    }
    
    void setPhase(int index, float phaseValue)
    {
        rotationValue[index].phase = phaseValue;
        repaint();
    }
    
    void setAnimationValue(float animationValue)
    {
        this->animationValue = animationValue;
    }
    
    void setWheelPosition()
    {
        auto bounds = getLocalBounds().toFloat();
        float wheelWidth = bounds.getWidth() * 0.75f;
        float wheelMargin = bounds.getWidth() * 0.125f;
        float height = bounds.getHeight();
        float x = bounds.getX();
        float y = bounds.getY();
        
        float yOffset = animationValue * bounds.getHeight() * 2.0f;
        
        rotationValue[0].bounds.setBounds(x + wheelMargin, y - yOffset, wheelWidth, wheelWidth);
        rotationValue[1].bounds.setBounds(x + wheelMargin, y + height * 0.075f - yOffset, wheelWidth, wheelWidth);
        rotationValue[2].bounds.setBounds(x + wheelMargin, y + height * 0.15f - yOffset, wheelWidth, wheelWidth);

        float sumYScale = animationValue * bounds.getHeight() * 0.25f;
        float sumYOffset = (1.0f - animationValue) * bounds.getHeight() * 0.45f;
        
        rotationValue[0].sumBounds.setBounds(x + wheelMargin, y - sumYScale + sumYOffset, wheelWidth, wheelWidth);
        rotationValue[1].sumBounds.setBounds(x + wheelMargin, y - sumYScale + sumYOffset, wheelWidth, wheelWidth);
        rotationValue[2].sumBounds.setBounds(x + wheelMargin, y - sumYScale + sumYOffset, wheelWidth, wheelWidth);
        
    }

    juce::Path generateWheelPath(juce::Graphics& g, int index, bool isSum)
    {

        float x = isSum ? rotationValue[index].sumBounds.getX() : rotationValue[index].bounds.getX();
        float y = isSum ? rotationValue[index].sumBounds.getY() : rotationValue[index].bounds.getY();
        float width = isSum ? rotationValue[index].sumBounds.getWidth() : rotationValue[index].bounds.getWidth();
        float height = isSum ? rotationValue[index].sumBounds.getHeight() : rotationValue[index].bounds.getHeight();
                
        int ratio = rotationValue[index].ratio;
        if (ratio < 0) { ratio = 1; }
        
        float twopi = juce::MathConstants<float>::twoPi;
        float radius = width/2;
        
        juce::Path wheelPath;
        for(int i = 0; i < ratio * 2 - 1; i+= 2){
            
            float startAngle = rotationValue[index].angles[i] * twopi;
            float endAngle = rotationValue[index].angles[i + 1] * twopi;

            if (endAngle <= startAngle)
                endAngle += twopi;
            if (i % 2 == 0){
                
                wheelPath.startNewSubPath(x + width/2, y + height/(isometricSkew)); // center
                wheelPath.addCentredArc(x + width/2, y + height/(isometricSkew),
                                        radius, radius/(isometricSkew),
                                        0.0f, startAngle, endAngle);
                wheelPath.closeSubPath();
                wheelPath = wheelPath.createPathWithRoundedCorners(4);
            }
        }
        return wheelPath;
    }



private:
    juce::Colour colorA, colorB, colorC, colorAB, colorBC, colorAC, colorABC;
    
    int overlapIndex;
    float animationValue;
    float isometricSkew;
    float yOffset, yPosOffset;
    Palette p;
};


