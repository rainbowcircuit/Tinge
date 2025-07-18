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

class DialGraphics : public juce::LookAndFeel_V4, DrawHelper
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
        float graphicWidth = bounds.getWidth();
        float graphicHeight = bounds.getHeight();

        if(index == 0){
            // rate
            drawRate(g, xPos, yPos, graphicWidth, graphicHeight, sliderPosProportional);
            
        } else if (index == 1){
            
   //         size = size/2;
            drawDivision(g, xPos, yPos, graphicWidth, graphicHeight, sliderPosProportional);

        } else if (index == 2){
            drawPhaseSlider(g, xPos, yPos, size, sliderPosProportional);
        } else if (index == 3){
            float position = sliderPosProportional * 7;
            position = std::floor(position);
            drawOverlapSlider(g, xPos, yPos, size, position);
        }
    }
    
    void drawOverlapSlider(juce::Graphics &g, float x, float y, float size, int index)
    {
        juce::Path pathA, pathB, pathC;
        float centerX = x + size/2;
        float centerY = y + size/2;
        float offset = size * 0.1f;
        float radius = size * 0.2f;

        pathA.addCentredArc(centerX - offset, centerY + offset, radius, radius, 0.0f, 0.0f, 6.28f, true);
        pathB.addCentredArc(centerX + offset, centerY + offset, radius, radius, 0.0f, 0.0f, 6.28f, true);
        pathC.addCentredArc(centerX, centerY - offset, radius, radius, 0.0f, 0.0f, 6.28f, true);
        
        if (index == 0)
        {
            drawWithoutOverlap(g, pathA, Colors::graphicRed);
            drawWithoutOverlap(g, pathB, Colors::graphicGreen);
            drawWithoutOverlap(g, pathC, Colors::graphicBlue);
            
            drawDoubleOverlap(g, pathA, pathB, Colors::backgroundFillAlt);
            drawDoubleOverlap(g, pathB, pathC, Colors::backgroundFillAlt);
            drawDoubleOverlap(g, pathA, pathC, Colors::backgroundFillAlt);
            drawTripleOverlap(g, pathA, pathB, pathC, Colors::backgroundFillAlt);
        } else if (index == 1)
        {
            drawWithoutOverlap(g, pathA, Colors::backgroundFillAlt);
            drawWithoutOverlap(g, pathB, Colors::backgroundFillAlt);
            drawWithoutOverlap(g, pathC, Colors::backgroundFillAlt);
            
            drawDoubleOverlap(g, pathA, pathB, Colors::graphicYellow);
            drawDoubleOverlap(g, pathB, pathC, Colors::graphicCyan);
            drawDoubleOverlap(g, pathA, pathC, Colors::graphicMagenta);
            drawTripleOverlap(g, pathA, pathB, pathC, Colors::backgroundFillAlt);
            
        } else if (index == 2)
        {
            drawWithoutOverlap(g, pathA, Colors::backgroundFill);
            drawWithoutOverlap(g, pathB, Colors::backgroundFill);
            drawWithoutOverlap(g, pathC, Colors::backgroundFill);

            drawDoubleOverlap(g, pathA, pathB, Colors::backgroundFill);
            drawDoubleOverlap(g, pathB, pathC, Colors::backgroundFill);
            drawDoubleOverlap(g, pathA, pathC, Colors::backgroundFill);
            drawTripleOverlap(g, pathA, pathB, pathC, Colors::graphicWhite);

        } else if (index == 3)
        {
            drawWithoutOverlap(g, pathA, Colors::graphicRed);
            drawWithoutOverlap(g, pathB, Colors::graphicGreen);
            drawWithoutOverlap(g, pathC, Colors::graphicBlue);

            drawDoubleOverlap(g, pathA, pathB, Colors::graphicYellow);
            drawDoubleOverlap(g, pathB, pathC, Colors::graphicCyan);
            drawDoubleOverlap(g, pathA, pathC, Colors::graphicMagenta);
            drawTripleOverlap(g, pathA, pathB, pathC, Colors::backgroundFill);

        } else if (index == 4)
        {
            drawWithoutOverlap(g, pathA, Colors::graphicRed);
            drawWithoutOverlap(g, pathB, Colors::graphicGreen);
            drawWithoutOverlap(g, pathC, Colors::graphicBlue);

            drawDoubleOverlap(g, pathA, pathB, Colors::backgroundFill);
            drawDoubleOverlap(g, pathB, pathC, Colors::backgroundFill);
            drawDoubleOverlap(g, pathA, pathC, Colors::backgroundFill);
            drawTripleOverlap(g, pathA, pathB, pathC, Colors::graphicWhite);
        } else if (index == 5)
        {
            drawWithoutOverlap(g, pathA, Colors::backgroundFill);
            drawWithoutOverlap(g, pathB, Colors::backgroundFill);
            drawWithoutOverlap(g, pathC, Colors::backgroundFill);

            drawDoubleOverlap(g, pathA, pathB, Colors::graphicYellow);
            drawDoubleOverlap(g, pathB, pathC, Colors::graphicCyan);
            drawDoubleOverlap(g, pathA, pathC, Colors::graphicMagenta);
            drawTripleOverlap(g, pathA, pathB, pathC, Colors::graphicWhite);
        } else if (index == 6)
        {
            drawWithoutOverlap(g, pathA, Colors::graphicRed);
            drawWithoutOverlap(g, pathB, Colors::graphicGreen);
            drawWithoutOverlap(g, pathC, Colors::graphicBlue);

            drawDoubleOverlap(g, pathA, pathB, Colors::graphicYellow);
            drawDoubleOverlap(g, pathB, pathC, Colors::graphicCyan);
            drawDoubleOverlap(g, pathA, pathC, Colors::graphicMagenta);
            drawTripleOverlap(g, pathA, pathB, pathC, Colors::graphicWhite);
        }
        
        g.setColour(Colors::backgroundFill);
        g.strokePath(pathA, juce::PathStrokeType(1.5f));
        g.strokePath(pathB, juce::PathStrokeType(1.5f));
        g.strokePath(pathC, juce::PathStrokeType(1.5f));


    }
    
    
    /*
    void drawRateMeasurement(juce::Graphics &g, float x, float y, float width, float height, float position)
    {
        
        float graphicWidth = width * 0.9f;
        float widthMargin = width * 0.05f;
        float graphicHeight = height * 0.9f;
        float heightMargin = width * 0.05f;

        juce::PathStrokeType strokeType(1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);

        juce::Point centerTopCoords = { x + widthMargin + graphicWidth * position, y + heightMargin};
        juce::Point centerBotCoords = { x + widthMargin + graphicWidth * position, y + heightMargin + graphicHeight };
        
        g.setColour(Colors::graphicWhite);
        juce::Path centerPath;
        centerPath.startNewSubPath(centerTopCoords);
        centerPath.lineTo(centerBotCoords);
        g.strokePath(centerPath, strokeType);
        centerPath.addCentredArc(centerTopCoords.x, centerTopCoords.y, 3.0f, 3.0f, 0.0f, 0.0f, 6.28f, true);
        g.fillPath(centerPath);
        

        for (int i = 0 ; i < 9; i++)
        {
            juce::Path measurementPath;

            if (i < 4)
            {
                float measurementIncr = (graphicWidth * position)/4;
                juce::Point measurementTopCoords = { x + widthMargin + measurementIncr * i, y + heightMargin + graphicHeight * 0.5f };
                juce::Point measurementBotCoords = { x + widthMargin + measurementIncr * i, y + heightMargin + graphicHeight};
                
                measurementPath.startNewSubPath(measurementTopCoords);
                measurementPath.lineTo(measurementBotCoords);
                g.strokePath(measurementPath, strokeType);
                
            } else if (i > 4) {
                int j = i - 4;
                float widthOffset = graphicWidth * position;
                float measurementIncr = ((graphicWidth * (1.0f - position))/4) * j;

                juce::Point measurementTopCoords = { x + widthMargin + widthOffset + measurementIncr, y + heightMargin + graphicHeight * 0.5f };
                juce::Point measurementBotCoords = { x + widthMargin + widthOffset + measurementIncr, y + heightMargin + graphicHeight};
                
                measurementPath.startNewSubPath(measurementTopCoords);
                measurementPath.lineTo(measurementBotCoords);
                g.strokePath(measurementPath, strokeType);
            }
        }
        
        juce::Path bottomPath;
        bottomPath.startNewSubPath(x + widthMargin, y + heightMargin + graphicHeight - 2);
        bottomPath.lineTo(x + widthMargin + graphicWidth, y + heightMargin + graphicHeight - 2);
        g.strokePath(bottomPath, strokeType);

    }
    */
    
    void drawRate(juce::Graphics &g, float x, float y, float width, float height, float position)
    {
        juce::Point<float> leftCoords = { x + width * 0.025f, y + height * 0.9f };
        juce::Point<float> rightCoords = { x + width * 0.975f, y + height * 0.9f };

        juce::Point<float> midCoords = {
            x + (width * 0.025f) + (width * 0.95f * position),
            y + height * 0.025f };
        juce::Point<float> midLeftCoords = {
            juce::jlimit(x + width * 0.025f, x + width * 0.975f, midCoords.x - width * 0.15f),
            y + height * 0.9f };
        juce::Point<float> midRightCoords = {
            juce::jlimit(x + width * 0.025f, x + width * 0.975f, midCoords.x + width * 0.15f),
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
    
    juce::Point<float> interpolateToVanishing(juce::Point<float> origin, juce::Point<float> vanPoint)
    {
        
        juce::Point<float> interpolatedPoint {
            origin.x + (vanPoint.x - origin.x) * 0.2f,
            origin.y + (vanPoint.y - origin.y) * 0.2f };
        
        return interpolatedPoint;
    }
    
    void drawDivision(juce::Graphics &g, float x, float y, float width, float height, float position)
    {
        float division = position * 4;
        division = std::floor(division) + 1;

        float widthIncr = width * 0.9f/4;
        float widthMargin = (width * 0.05f);
        float graphicHeight = height * 0.6f;
        float heightMargin = height * 0.2f;
        
        float vpAdjust = width * 0.75f * (position - 0.5f);
        juce::Point<float> vanishingPoint = { (x + width/2) + vpAdjust, y + height/2 };
        
        for (int i = 0; i < 5; i++)
        {
            juce::Point<float> frontTopCoords = { x + widthMargin + widthIncr * i, y + heightMargin };
            juce::Point<float> frontBotCoords  = { x + widthMargin  + widthIncr * i, y + heightMargin + graphicHeight };
            juce::Point<float> backTopCoords = interpolateToVanishing(frontTopCoords, vanishingPoint);
            juce::Point<float> backBotCoords = interpolateToVanishing(frontBotCoords, vanishingPoint);

            juce::Path frontPath, bodyPath;
            bodyPath.startNewSubPath(frontTopCoords);
            bodyPath.lineTo(frontBotCoords);
            bodyPath.lineTo(backBotCoords);
            bodyPath.lineTo(backTopCoords);
            juce::Colour fillColour = (i >= division) ? juce::Colour(140, 140, 140) : Colors::graphicWhite;
            g.setColour(fillColour);
            g.fillPath(bodyPath);
            
            // front lines
            frontPath.startNewSubPath(frontTopCoords);
            frontPath.lineTo(frontBotCoords);
            g.setColour(fillColour);
            juce::PathStrokeType thickStroke(1.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
            g.strokePath(frontPath, juce::PathStrokeType(thickStroke));

        }
    }
    

    /*
    void drawDivision(juce::Graphics &g, float x, float y, float size, float division, bool isSelected)
    {
        float radius = size * 0.35f;
        float twopi = juce::MathConstants<float>::twoPi;
                
        float segmentSize = twopi/division * 2;
        
        for(int i = 0; i < division; i += 2)
        {
            float startAngle = i * segmentSize;
            float endAngle = (i + 1) * segmentSize;

            juce::Path graphicPath;
                graphicPath.startNewSubPath(x + size/2, (y + size/2));
                graphicPath.addCentredArc(x + size/2, (y + size/2), radius, radius, 0.0f, startAngle, endAngle);
                graphicPath.closeSubPath();

            if (i % 2 == 0) {
                juce::Colour selection = isSelected ? juce::Colour(200, 200, 200) : juce::Colour(150, 150, 150);
                g.setColour(selection);
                g.fillPath(graphicPath);
                
            } else if (i % 2 == 1) {
                g.setColour(juce::Colour(125, 125, 125));
                g.fillPath(graphicPath);

            }
        }
    }
    */
     
    void drawPhaseSlider(juce::Graphics &g, float x, float y, float size, float position)
    {
        /*
        int domainResolution = 128;
        float width = size * 0.9f;
        float widthMargin = size * 0.05f;
        float widthIncrement = width/domainResolution;
        
        juce::Path graphicPath;
        graphicPath.startNewSubPath(x + widthMargin, y + size/2);
        for (int i = 0; i < domainResolution; i++)
        {
            float sin = 1.0f * std::sin((i/40.7f) * 2.0f);
            graphicPath.lineTo(x + widthMargin + widthIncrement * i,
                               (y + size/2) + (size * sin/2) * 0.8f);
        }
    
        graphicPath = graphicPath.createPathWithRoundedCorners(4.0f);
        juce::PathStrokeType strokeType(2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
        g.setColour(juce::Colour(150, 150, 150));
        g.strokePath(graphicPath, strokeType);
        
        float sin = std::sin(position * 6.28f);
        juce::Point positionCoords = { x + widthMargin + width * position, (y + size/2) + (size * sin/2) * 0.8f };

        juce::Path pointPath;
        pointPath.addCentredArc(positionCoords.x, positionCoords.y, 3.0f, 3.0f, 0.0f, 0.0f, 6.28f, true);
        g.setColour(juce::Colour(200, 200, 200)); //
        g.fillPath(pointPath);
         */
    }
    
    

    void drawRoundDial(juce::Graphics &g, float x, float y, float size, float position)
    {
        //==============================================================================

        float pi = juce::MathConstants<float>::pi;
        float dialStart = 1.25f * pi;
        float dialEnd = 2.75f * pi;
        float sliderPositionScaled = 2.0f + (1.0f - position);
        float dialPositionInRadians = dialStart + sliderPositionScaled * (dialEnd - dialStart);
        

        juce::Path dialBodyPath, dialDotPath, dialOutlinePath, dialSelectPath, tensionLeftPath, tensionRightPath;
        
        float dialOutlineRadius = (size * 0.8f)/2;
        float dialBodyRadius = (size * 0.7f)/2;
        float dialDotRadius = (size * 0.5f)/2;

        dialOutlinePath.addCentredArc(x + size/2, x + size/2,
                                      dialOutlineRadius, dialOutlineRadius,
                                      0.0f, dialStart, dialEnd, true);
        g.setColour(juce::Colour(200, 200, 200)); //

        juce::PathStrokeType strokeType(2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
        g.strokePath(dialOutlinePath, juce::PathStrokeType(strokeType));

        //==============================================================================
        // dial body
        
        dialBodyPath.addCentredArc(x + size/2, y + size/2,
                                   dialBodyRadius, dialBodyRadius,
                                   0.0f, 0.0f, 6.28f, true);
        g.setColour(juce::Colour(200, 200, 200)); //
        g.fillPath(dialBodyPath);
        
        //==============================================================================
        // dial dot
        
        juce::Point<float> outlineCoords = {x + size/2 + std::sin(dialPositionInRadians) * dialDotRadius,
            x + size/2 + std::cos(dialPositionInRadians) * dialDotRadius};

        dialDotPath.addCentredArc(outlineCoords.x, outlineCoords.y,
                                  1.5, 1.5, 0.0f, 0.0f, pi * 2, true);
        
        g.setColour(juce::Colour(20, 20, 20)); //
        g.fillPath(dialDotPath);
    }
    
private:
    int index;
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
            drawNudgeButton(g, x, y, width, shouldDrawButtonAsDown, false);
            
        } else if (graphicIndex == 1)
        {
            drawNudgeButton(g, x, y, width, shouldDrawButtonAsDown, true);
        } else if (graphicIndex == 2)
        {
            drawBrakeButton(g, x, y, width, shouldDrawButtonAsDown);
        }

    }
    
    void drawNudgeButton(juce::Graphics& g, float x, float y, float size, bool buttonDown, bool mirror)
    {
        
        juce::Rectangle<float> baseRect { x + size * 0.15f, y + size * 0.15f, size * 0.5f, size * 0.7f };
        float offset = baseRect.getWidth() * 0.5f;
        float rectSize = baseRect.getHeight();

        juce::Point<float> midPoint = { baseRect.getTopRight().x + offset, baseRect.getTopRight().y + rectSize * 0.5f };
        juce::Point<float> midTopPoint = { baseRect.getTopRight().x, baseRect.getTopRight().y + rectSize * 0.15f };
        juce::Point<float> midBotPoint = { baseRect.getTopRight().x, baseRect.getTopRight().y + rectSize * 0.85f };

        juce::Path graphicPath;

        
        graphicPath.addRoundedRectangle(baseRect, 3.0f);
        graphicPath.startNewSubPath(baseRect.getTopRight());
        graphicPath.lineTo(midTopPoint);
        graphicPath.lineTo(midPoint);
        graphicPath.lineTo(midBotPoint);
        graphicPath.lineTo(baseRect.getBottomRight());
        graphicPath.closeSubPath();
        graphicPath = graphicPath.createPathWithRoundedCorners(size * 0.5f);
        
        if (mirror)
        {
            float centerX = x + size * 0.5f;
            juce::AffineTransform mirrorTransform =
                juce::AffineTransform::translation(-centerX, 0.0f)
                .scaled(-1.0f, 1.0f)
                .translated(centerX, 0.0f);
            graphicPath.applyTransform(mirrorTransform);
        }
        
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

        
    }
    
    void drawBrakeButton(juce::Graphics& g, float x, float y, float size, bool buttonDown)
    {
        
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

    }

private:
    int graphicIndex;
};




class EditableTextBoxSlider : public juce::Component , juce::AudioProcessorParameter::Listener, juce::AsyncUpdater, juce::Label::Listener
{
public:
    EditableTextBoxSlider(TingeAudioProcessor& p, juce::String parameterID, juce::String parameterSuffix) : audioProcessor(p)
    {
        this->parameterID = parameterID;
        this->parameterSuffix = parameterSuffix;
        
        addAndMakeVisible(textBox);
        textBox.setEditable(false, false, false);
        textBox.setInterceptsMouseClicks(false, false);
        
        // initialize displayed value
        auto value = audioProcessor.apvts.getRawParameterValue(parameterID)->load();
        juce::String formattedValue = juce::String(value, 2) + parameterSuffix;
        textBox.setText(formattedValue, juce::dontSendNotification);
        textBox.addListener(this);
        
        // initialize parameter ranges
        juce::NormalisableRange range = audioProcessor.apvts.getParameterRange(parameterID);
        rangeStart = range.start;
        rangeEnd = range.end;
        
        // add listener
        const auto params = audioProcessor.getParameters();
        for (auto param : params){
            param->addListener(this);
        }
    }
    
    ~EditableTextBoxSlider()
    {
        const auto params = audioProcessor.getParameters();
        for (auto param : params){
            param->removeListener(this);
        }
    }
    
    void setSliderMode(bool displayChoice, bool sliderIsTime)
    {
        this->displayChoice = displayChoice;
        this->sliderIsTime = sliderIsTime;
    }
    
    void paint(juce::Graphics& g) override {}
    
    void resized() override {
        auto bounds = getLocalBounds();
        textBox.setBounds(bounds);
        
    }
    
    void mouseDown(const juce::MouseEvent& m) override
    {
        auto mousePoint = m.getPosition().toFloat();
        dragStartPoint.y = mousePoint.y;
    }

    void mouseDrag(const juce::MouseEvent& m) override
    {
        auto mousePoint = m.getPosition().toFloat();
        float deltaY = std::abs(mousePoint.y - dragStartPoint.y);

        float value = juce::jlimit(0.0f, 1.0f, deltaY/100.0f); // clamp this
        textValueToParamValue(value);
    }
    
    void mouseUp(const juce::MouseEvent& m) override
    {
        auto mousePoint = m.getPosition().toFloat();
        dragStartPoint.y = mousePoint.y;
    }
    
    void mouseDoubleClick(const juce::MouseEvent& m) override
    {
        textBox.setInterceptsMouseClicks(true, true);
        textBox.setEditable(true, true, false);
        textBox.showEditor();
        textBox.grabKeyboardFocus();
    }
    
    void labelTextChanged(juce::Label* l) override
    {
        auto value = l->getText().getFloatValue();
        float valueLimited = juce::jlimit(rangeStart, rangeEnd, value);
        
        l->setText(juce::String(valueLimited, 2), juce::dontSendNotification);
        textBox.setInterceptsMouseClicks(false, false);
        
        float normalized = (valueLimited - rangeStart) / (rangeEnd - rangeStart);
        textValueToParamValue(normalized);
        repaint();
    }

    void textValueToParamValue(float value)
    {
        value = juce::jlimit(0.0f, 1.0f, value);
        audioProcessor.apvts.getParameter(parameterID)->setValueNotifyingHost(value);
    }
        
    void parameterValueChanged (int parameterIndex, float newValue) override
    {
        newValueAtomic.store(newValue);
        parameterIndexAtomic.store(parameterIndex);
        triggerAsyncUpdate();
    }
    
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override {}
    
    void handleAsyncUpdate() override
    {
        float newValue = newValueAtomic.load();
        int parameterIndex = parameterIndexAtomic.load();
        juce::String newParameterID;
        juce::StringArray choiceValue;
        float scaledValue;
        
        if (auto* param = dynamic_cast<juce::AudioProcessorParameterWithID*>(audioProcessor.getParameters()[parameterIndex]))
        {
            if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(param))
            {
                scaledValue = rangedParam->convertFrom0to1(newValue);
                choiceValue = rangedParam->getAllValueStrings();
                newParameterID = param->paramID;
            }
        }
        
        if (newParameterID == parameterID)
        {
            juce::String formattedValue = juce::String(scaledValue, 2) + parameterSuffix;
            if (displayChoice)
            {
                formattedValue = choiceValue[scaledValue];
            }

            textBox.setText(formattedValue, juce::dontSendNotification);
        }
    }

    void setFontSize(float size)
    {
        textBox.setFont(juce::FontOptions(size, juce::Font::plain));
    }
    
private:
    float rangeStart, rangeEnd;
    std::atomic<float> newValueAtomic;
    std::atomic<int> parameterIndexAtomic;
    
    juce::Point<float> dragStartPoint;
    juce::Label textBox;
    juce::String parameterID, parameterSuffix = "";
    bool displayChoice, sliderIsTime;
    
    TingeAudioProcessor& audioProcessor;
};
