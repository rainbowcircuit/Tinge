/*
  ==============================================================================

    GraphicsHelper.h
    Created: 19 Aug 2025 9:20:31pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "LookAndFeel.h"

class GraphicsHelper
{
public:
    void setSlider(juce::Component& parent, juce::Slider& slider, juce::LookAndFeel& lookAndFeel)
    {
        parent.addAndMakeVisible(slider);
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setLookAndFeel(&lookAndFeel);
    }
        
    void setButton(juce::Component& parent, juce::TextButton& button, juce::LookAndFeel& lookAndFeel)
    {
        parent.addAndMakeVisible(button);
        button.setLookAndFeel(&lookAndFeel);
    }

    
    void setLabel(juce::Component& parent, juce::Label& label, juce::String labelText, juce::Colour textColor, juce::Justification justification)
    {
        juce::FontOptions font(12.0f, juce::Font::plain);
        parent.addAndMakeVisible(label);
        label.setText(labelText, juce::dontSendNotification);
        label.setColour(juce::Label::textColourId, juce::Colour(80, 80, 80));
        label.setJustificationType(justification);
        label.setColour(juce::Label::textColourId, textColor);
        label.setFont(font);
    }
    
    void setFontSize(float size)
    {
        fontSize = size;
    }

    float getFontSize()
    {
        return fontSize;
    }

    void setMargin(float margin)
    {
        this->margin = margin;
    }
    
    int getMargin()
    {
        return margin;
    }
    
    juce::Label* createTextBox(juce::Slider& s)
    {
        juce::LookAndFeel_V4 laf;
        auto l = laf.createSliderTextBox(s);
        
        l->setFont(juce::FontOptions(fontSize, juce::Font::plain));
        l->setJustificationType(juce::Justification::centred);
        l->setColour(juce::Label::textColourId, Colors::graphicBlack);

        l->onEditorShow = [l]()
        {
            if(auto* editor = l->getCurrentTextEditor())
            {
                editor->setJustification(juce::Justification::centred);
            }
        };
        return l;
    }

private:
    float fontSize = 12.0f;
    int margin = 5;

};

class DrawHelper
{
public:
    
    void drawWithoutOverlap(juce::Graphics& g, juce::Path& path, juce::Colour color, bool strokePath)
    {
        g.setColour(color);
        g.fillPath(path);
        if (strokePath){
            g.setColour(Colors::backgroundFill);
            g.strokePath(path, juce::PathStrokeType(1.5f));
        }
    }

    void drawDoubleOverlap(juce::Graphics& g, juce::Path& pathA, juce::Path& pathB, juce::Colour color, bool strokePath)
    {
        g.saveState();
        g.reduceClipRegion(pathA);
        g.reduceClipRegion(pathB);
        g.setColour(color);
        g.fillAll();
        if (strokePath){
            g.setColour(Colors::backgroundFill);
            g.strokePath(pathA, juce::PathStrokeType(1.5f));
        }
        g.restoreState();
    }

    void drawTripleOverlap(juce::Graphics& g, juce::Path& pathA, juce::Path& pathB, juce::Path& pathC, juce::Colour color, bool strokePath)
    {
        g.saveState();
        g.reduceClipRegion(pathA);
        g.reduceClipRegion(pathB);
        g.reduceClipRegion(pathC);
        g.setColour(color);
        g.fillAll();
        if (strokePath){
            g.setColour(Colors::backgroundFill);
            g.strokePath(pathA, juce::PathStrokeType(1.5f));
        }
        g.restoreState();
    }
    
    void drawQuadrupleOverlap(juce::Graphics& g, juce::Path& pathA, juce::Path& pathB, juce::Path& pathC, juce::Path& pathD, juce::Colour color, bool strokePath)
    {
        g.saveState();
        g.reduceClipRegion(pathA);
        g.reduceClipRegion(pathB);
        g.reduceClipRegion(pathC);
        g.reduceClipRegion(pathD);
        g.setColour(color);
        g.fillAll();
        if (strokePath){
            g.setColour(Colors::backgroundFill);
            g.strokePath(pathA, juce::PathStrokeType(1.5f));
        }
        g.restoreState();
    }

};
