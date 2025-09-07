/*
  ==============================================================================

    UserInterfaceLookAndFeel.h
    Created: 3 Jul 2025 12:51:07pm
    Author:  Takuma Matsui

  ==============================================================================
*/
#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookAndFeel.h"
#include "Graphics.h"

/*

class DraftGraphics : public juce::LookAndFeel_V4, DrawHelper
{
public:
    DraftGraphics(int index)
    {
        this->index = index;
    }
    
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider) override
    {
        auto bounds = slider.getLocalBounds().toFloat();
        bounds.reduce(5, 5);
        float xPos = bounds.getX();
        float yPos = bounds.getY();
        float size = bounds.getWidth();
        float graphicWidth = bounds.getWidth();
        float graphicHeight = bounds.getHeight();
        
        if(index == 0){
            drawRate(g, xPos, yPos, graphicWidth, graphicHeight, sliderPosProportional);
            
        } else if (index == 1){
            drawRatio(g, xPos, yPos, size, sliderPosProportional);

        } else if (index == 2){
            drawOpacity(g, xPos, yPos, size, sliderPosProportional);

        } else if (index == 3){
            float position = sliderPosProportional * 6;
            position = std::floor(position);
            
        } else if (index == 4){
            
        }
    }
    
    void drawRate(juce::Graphics& g, float x, float y, float width, float height, float pos)
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
            sinMainPath = sinMainPath.createPathWithRoundedCorners(3);
            
            offsetLPath.lineTo(offsetLCoords.x, offsetLCoords.y);
            offsetLPath = offsetLPath.createPathWithRoundedCorners(3);

            offsetRPath.lineTo(offsetRCoords.x, offsetRCoords.y);
            offsetRPath = offsetRPath.createPathWithRoundedCorners(3);

            g.setColour(juce::Colour(80, 80, 80));
            g.strokePath(offsetLPath, juce::PathStrokeType(1.5f));
            
            g.setColour(juce::Colour(130, 130, 130));
            g.strokePath(offsetRPath, juce::PathStrokeType(1.5f));
            
            g.setColour(juce::Colour(190, 190, 190));
            g.strokePath(sinMainPath, juce::PathStrokeType(1.5f));
        }
    }

    void drawRatio(juce::Graphics& g, float x, float y, float size, float index)
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

    void drawOpacity(juce::Graphics& g, float x, float y, float size, float pos)
    {

        float radius = size * 0.25f;
        float offset = (1.0f - pos) * radius;

        juce::Path leftCircle, rightCircle;
        juce::Point centerCoords = { x + size/2, y + size/2 };
        
        leftCircle.addCentredArc(centerCoords.x - offset, centerCoords.y, radius, radius, 0.0f, 0.0f, 6.28f, true);
        rightCircle.addCentredArc(centerCoords.x + offset, centerCoords.y, radius, radius, 0.0f, 0.0f, 6.28f, true);
        g.setColour(juce::Colour(190, 190, 190));
        g.strokePath(leftCircle, juce::PathStrokeType(1.5f));
        g.strokePath(rightCircle, juce::PathStrokeType(1.5f));

        drawDoubleOverlap(g, leftCircle, rightCircle, juce::Colour(190, 190, 190), false);
        
    }
    
    int index;
    juce::Colour iconShade = juce::Colour(190, 190, 190);
    juce::Colour iconDisabled = juce::Colour(90, 90, 90);
};

*/
class DialGraphics : public juce::LookAndFeel_V4, DrawHelper
{
public:
    DialGraphics(int index)
    {
        this->index = index;
    }
    
    void setColor(float opacityA, float opacityB, float opacityC)
    {
        p.setColors(opacityA, opacityB, opacityC);
    }
    
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider) override
    {
        auto bounds = slider.getLocalBounds().toFloat();
        bounds.reduce(5, 5);
        float xPos = bounds.getX();
        float yPos = bounds.getY();
        float size = bounds.getWidth();
        float graphicWidth = bounds.getWidth();
        float graphicHeight = bounds.getHeight();

        if(index == 0){
            drawRate(g, xPos, yPos, graphicWidth, graphicHeight, sliderPosProportional);
            
        } else if (index == 1){

        } else if (index == 2){
            drawVerticalSlider(g, xPos, yPos, graphicWidth, graphicHeight, sliderPosProportional);

        } else if (index == 3){
            float position = sliderPosProportional * 6;
            position = std::floor(position);
            drawOverlapSlider(g, xPos, yPos, size, position);

        } else if (index == 4){
            
        }
    }
    
    void drawOverlapSlider(juce::Graphics &g, float x, float y, float size, int overlapIndex)
    {
        juce::Path pathA, pathB, pathC;
        float centerX = x + size/2;
        float centerY = y + size/2;
        float offset = size * 0.1f;
        float radius = size * 0.2f;

        pathA.addCentredArc(centerX - offset, centerY + offset, radius, radius, 0.0f, 0.0f, 6.28f, true);
        pathB.addCentredArc(centerX + offset, centerY + offset, radius, radius, 0.0f, 0.0f, 6.28f, true);
        pathC.addCentredArc(centerX, centerY - offset, radius, radius, 0.0f, 0.0f, 6.28f, true);
        
        juce::Colour colorA, colorB, colorC, colorAB, colorBC, colorAC, colorABC;
        
        if (overlapIndex == 0 || overlapIndex == 3 || overlapIndex == 4 || overlapIndex == 6){
            colorA = p.colorA;
            colorB = p.colorB;
            colorC = p.colorC;
        } else {
            colorA = Colors::backgroundFillAlt;
            colorB = Colors::backgroundFillAlt;
            colorC = Colors::backgroundFillAlt;
        }
        
        if (overlapIndex == 1 || overlapIndex == 3 || overlapIndex == 5 || overlapIndex == 6){
            colorAB = p.colorAB;
            colorBC = p.colorBC;
            colorAC = p.colorAC;
        } else {
            colorAB = Colors::backgroundFillAlt;
            colorBC = Colors::backgroundFillAlt;
            colorAC = Colors::backgroundFillAlt;
        }
        
        if (overlapIndex == 2 || overlapIndex == 4 || overlapIndex == 5 || overlapIndex == 6){
            colorABC = p.colorABC;
        } else {
            colorABC = Colors::backgroundFill;
        }
        
        drawWithoutOverlap(g, pathA, colorA, true);
        drawWithoutOverlap(g, pathB, colorB, true);
        drawWithoutOverlap(g, pathC, colorC, true);
        drawDoubleOverlap(g, pathA, pathB, colorAB, true);
        drawDoubleOverlap(g, pathB, pathC, colorBC, true);
        drawDoubleOverlap(g, pathA, pathC, colorAC, true);
        drawTripleOverlap(g, pathA, pathB, pathC, colorABC, true);
        
  //     g.setColour(Colors::graphicBlack);
   //     g.strokePath(pathA, juce::PathStrokeType(1.5f));
    //    g.strokePath(pathB, juce::PathStrokeType(1.5f));
    //    g.strokePath(pathC, juce::PathStrokeType(1.5f));
    }
    
    void drawRate(juce::Graphics &g, float x, float y, float width, float height, float position)
    {
        juce::Point<float> leftCoords = { x + width * 0.025f, y + height * 0.9f };
        juce::Point<float> rightCoords = { x + width * 0.975f, y + height * 0.9f };

        juce::Point<float> midCoords = {
            x + (width * 0.175f) + (width * 0.65f * position),
            y + height * 0.025f };
        
        float positionScaled = 1.0f - (std::abs(position - 0.5f) * 2.0f);
        float widthScaled = (width * 0.15f * positionScaled) + (width * 0.05f);
        
        juce::Point<float> midLeftCoords = {
            juce::jlimit(x + width * 0.125f, x + width * 0.875f, midCoords.x - widthScaled),
            y + height * 0.9f };
        juce::Point<float> midRightCoords = {
            juce::jlimit(x + width * 0.125f, x + width * 0.875f, midCoords.x + widthScaled),
            y + height * 0.9f };
        
        juce::Path graphicLines;
        graphicLines.startNewSubPath(leftCoords);
        graphicLines.lineTo(midLeftCoords);
        graphicLines.lineTo(midCoords);
        graphicLines.lineTo(midRightCoords);
        graphicLines.lineTo(rightCoords);
        
        graphicLines = graphicLines.createPathWithRoundedCorners(18.0f);
                
        g.setColour(Colors::graphicWhite);
        juce::PathStrokeType thickStroke(2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
        g.strokePath(graphicLines, juce::PathStrokeType(thickStroke));

        
    }
        
    void drawVerticalSlider(juce::Graphics &g, float x, float y, float width, float height, float position)
    {
        juce::Path outlinePath, fillPath;
        g.setColour(Colors::graphicWhite);
        
        outlinePath.addRoundedRectangle(x, y, width, height, width * 0.15f);
        g.strokePath(outlinePath, juce::PathStrokeType(2.0f));
        
        float yPos = y + height * (1.0f - position);
        fillPath.addRoundedRectangle(x, yPos, width, height * position, width * 0.15f);
        g.fillPath(fillPath);
        g.strokePath(fillPath, juce::PathStrokeType(2.0f));
    }
    
    
private:
    int index;
    Palette p;
};


class ButtonGraphics : public juce::LookAndFeel_V4
{
public:
    ButtonGraphics(int graphicIndex)
    {
        this->graphicIndex = graphicIndex;
    }

    
    void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
    {
        auto bounds = juce::Rectangle<int>(button.getWidth(), button.getWidth()).toFloat();
        bounds.reduce(5, 5);
        float x = bounds.getX();
        float y = bounds.getY();
        float width = bounds.getWidth();
        float height = bounds.getHeight();
        
        if (graphicIndex == 0)
        {
            drawNudgeButton(g, x, y, width, shouldDrawButtonAsDown, true);
            
        } else if (graphicIndex == 1)
        {
            drawNudgeButton(g, x, y, width, shouldDrawButtonAsDown, false);
            
        } else if (graphicIndex == 2)
        {
            drawBrakeButton(g, x, y, width, shouldDrawButtonAsDown);
        }
    }

    void setLPGValue(float lpgValue)
    {
        this->lpgValue = lpgValue;
    }
    
    void drawNudgeButton(juce::Graphics& g, float x, float y, float size, bool buttonDown, bool mirror)
    {
        for (int i = 0; i < 5; i++)
        {
            float sizeIncr = (size/5) * (i * (1.0f - lpgValue));
            juce::Point<float> topPoint = { x + sizeIncr, y };
            juce::Point<float> botPoint = { x + sizeIncr, y + size };
            
            juce::Path graphicPath;
            graphicPath.startNewSubPath(topPoint);
            graphicPath.lineTo(botPoint);
            
            if (mirror)
            {
                float centerX = x + size * 0.5f;
                juce::AffineTransform mirrorTransform =
                    juce::AffineTransform::translation(-centerX, 0.0f)
                    .scaled(-1.0f, 1.0f)
                    .translated(centerX, 0.0f);
                graphicPath.applyTransform(mirrorTransform);
            }
            
            g.setColour(Colors::graphicWhite);
            g.strokePath(graphicPath, juce::PathStrokeType(1.5f));
        }
    }
    
    void drawBrakeButton(juce::Graphics& g, float x, float y, float size, bool buttonDown)
    {
        /*
        juce::Rectangle<float> rectangle { x + size * 0.15f, y + size * 0.15f, size * 0.7f, size * 0.7f };
        
        juce::Path graphicPath;
        graphicPath.addRoundedRectangle(rectangle, 3.0f);

        float transform = buttonDown ? 0.15f : 0.12f;

        // draw left
        juce::AffineTransform leftOffset = juce::AffineTransform::translation(size * transform, 0.0f);
        juce::Path leftGraphicPath = graphicPath;
        leftGraphicPath.applyTransform(leftOffset);
        g.setColour(Colors::graphicMagenta);
        g.fillPath(leftGraphicPath);
        
        // draw right
        juce::AffineTransform rightOffset = juce::AffineTransform::translation(size * -transform, 0.0f);
        juce::Path rightGraphicPath = graphicPath;
        rightGraphicPath.applyTransform(rightOffset);
        g.setColour(Colors::graphicCyan);
        g.fillPath(rightGraphicPath);
        
        // draw center
        juce::Colour mainFill = buttonDown ? Colors::graphicWhite : Colors::graphicWhiteAlt;
        g.setColour(mainFill);
        g.fillPath(graphicPath);
         */
    }

private:
    int graphicIndex;
    float lpgValue;
};

/*
class ColorButtonGraphics : public juce::LookAndFeel_V4
{
public:
    void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
    {
        auto bounds = juce::Rectangle<int>(button.getWidth(), button.getWidth()).toFloat();
        bounds.reduce(2, 2);
        
        float alpha = shouldDrawButtonAsDown ? 1.0f : 0.25f;
        g.setColour(Colors::gradientColor[colorButtonIndex].withAlpha((float)alpha));
        g.fillRoundedRectangle(bounds, 3);
    }
    
    void setColorButtonIndex(int index)
    {
        colorButtonIndex = index;
    }

private:
    int colorButtonIndex = 0;
};

*/
