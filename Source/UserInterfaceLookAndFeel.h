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
    
    void setColor(int colorA, int colorB, int colorC, float opacityA, float opacityB, float opacityC)
    {
        p.setColors(colorA, colorB, colorC, opacityA, opacityB, opacityC);
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
            drawRate(g, xPos, yPos, graphicWidth, graphicHeight, sliderPosProportional);
            
        } else if (index == 1){
            drawDivision(g, xPos, yPos, graphicWidth, graphicHeight, sliderPosProportional);

        } else if (index == 2){
       //    drawColorPicker(g, xPos, yPos, graphicWidth, graphicHeight, sliderPosProportional);

        } else if (index == 3){
            float position = sliderPosProportional * 7;
            position = std::floor(position);
            drawOverlapSlider(g, xPos, yPos, size, position);

        } else if (index == 4){
            
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
            drawWithoutOverlap(g, pathA, p.colorA);
            drawWithoutOverlap(g, pathB, p.colorB);
            drawWithoutOverlap(g, pathC, p.colorC);
            
            drawDoubleOverlap(g, pathA, pathB, Colors::backgroundFill);
            drawDoubleOverlap(g, pathB, pathC, Colors::backgroundFill);
            drawDoubleOverlap(g, pathA, pathC, Colors::backgroundFill);
            drawTripleOverlap(g, pathA, pathB, pathC, Colors::backgroundFill);
        } else if (index == 1)
        {
            drawWithoutOverlap(g, pathA, Colors::backgroundFill);
            drawWithoutOverlap(g, pathB, Colors::backgroundFill);
            drawWithoutOverlap(g, pathC, Colors::backgroundFill);
            
            drawDoubleOverlap(g, pathA, pathB, p.colorAB);
            drawDoubleOverlap(g, pathB, pathC, p.colorBC);
            drawDoubleOverlap(g, pathA, pathC, p.colorAC);
            drawTripleOverlap(g, pathA, pathB, pathC, Colors::backgroundFill);
            
        } else if (index == 2)
        {
            drawWithoutOverlap(g, pathA, Colors::backgroundFill);
            drawWithoutOverlap(g, pathB, Colors::backgroundFill);
            drawWithoutOverlap(g, pathC, Colors::backgroundFill);

            drawDoubleOverlap(g, pathA, pathB, Colors::backgroundFill);
            drawDoubleOverlap(g, pathB, pathC, Colors::backgroundFill);
            drawDoubleOverlap(g, pathA, pathC, Colors::backgroundFill);
            drawTripleOverlap(g, pathA, pathB, pathC, p.colorABC);

        } else if (index == 3)
        {
            drawWithoutOverlap(g, pathA, p.colorA);
            drawWithoutOverlap(g, pathB, p.colorB);
            drawWithoutOverlap(g, pathC, p.colorC);

            drawDoubleOverlap(g, pathA, pathB, p.colorAB);
            drawDoubleOverlap(g, pathB, pathC, p.colorBC);
            drawDoubleOverlap(g, pathA, pathC, p.colorAC);
            drawTripleOverlap(g, pathA, pathB, pathC, p.colorABC);

        } else if (index == 4)
        {
            drawWithoutOverlap(g, pathA, p.colorA);
            drawWithoutOverlap(g, pathB, p.colorB);
            drawWithoutOverlap(g, pathC, p.colorC);

            drawDoubleOverlap(g, pathA, pathB, p.colorAB);
            drawDoubleOverlap(g, pathB, pathC, p.colorBC);
            drawDoubleOverlap(g, pathA, pathC, p.colorAC);
            drawTripleOverlap(g, pathA, pathB, pathC, p.colorABC);
        } else if (index == 5)
        {
            drawWithoutOverlap(g, pathA, Colors::backgroundFill);
            drawWithoutOverlap(g, pathB, Colors::backgroundFill);
            drawWithoutOverlap(g, pathC, Colors::backgroundFill);

            drawDoubleOverlap(g, pathA, pathB, p.colorAB);
            drawDoubleOverlap(g, pathB, pathC, p.colorBC);
            drawDoubleOverlap(g, pathA, pathC, p.colorAC);
            drawTripleOverlap(g, pathA, pathB, pathC, p.colorABC);
        } else if (index == 6)
        {
            drawDoubleOverlap(g, pathA, pathB, p.colorAB);
            drawDoubleOverlap(g, pathB, pathC, p.colorBC);
            drawDoubleOverlap(g, pathA, pathC, p.colorAC);

            drawDoubleOverlap(g, pathA, pathB, p.colorAB);
            drawDoubleOverlap(g, pathB, pathC, p.colorBC);
            drawDoubleOverlap(g, pathA, pathC, p.colorAC);
            drawTripleOverlap(g, pathA, pathB, pathC, p.colorABC);
        }
        
        g.setColour(p.colorABC);
        g.strokePath(pathA, juce::PathStrokeType(1.5f));
        g.strokePath(pathB, juce::PathStrokeType(1.5f));
        g.strokePath(pathC, juce::PathStrokeType(1.5f));


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
    
    void drawDivision(juce::Graphics &g, float x, float y, float width, float height, float position)
    {
        // coordinates
        int positionScaled = std::floor(position * 4.0f + 1);
        float widthMargin = width * 0.05;
        width = width * 0.9;
        float segmentSize = width/positionScaled;

        float heightMargin = height * 0.05f;
        height = height * 0.9f;
        float blockMargin = segmentSize * 0.1f;
        float blockSize = segmentSize * 0.8f;
        //==============================================================================
        for (int i = 0; i < positionScaled; i++)
        {
            juce::Path graphicPath;
            float xPosition = widthMargin + blockMargin + i * (blockSize + blockMargin * 2);
            graphicPath.addRoundedRectangle(xPosition, heightMargin, blockSize, height, 3);
            graphicPath = graphicPath.createPathWithRoundedCorners(3);
            
            g.setColour(juce::Colour(255, 255, 255));
            g.fillPath(graphicPath);
        }
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
        
        /*
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

        */
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

