/*
  ==============================================================================

    HoldableButton.h
    Created: 9 Sep 2025 10:20:58pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "PluginProcessor.h"
#include "DSP.h"

enum class HoldableButtonLAF { NudgeForward, NudgeBackward, Brake };

class GlobalHoldableButton : public juce::Button, public juce::Timer
{
public:
    GlobalHoldableButton(TingeAudioProcessor& p, HoldableButtonLAF l)
    : juce::Button("HoldableButton"), lookAndFeel(l), audioProcessor(p)
    {
        slew.setSampleRate(60);
        slew.setEnvelopeSlew(240, 240);
        startTimerHz(60);
        
        switch(lookAndFeel){
            case HoldableButtonLAF::NudgeForward:
            {
                parameterID = "nudgeForward";
                break;
            }
            case HoldableButtonLAF::NudgeBackward:
            {
                parameterID = "nudgeBackward";
                break;
            }
            case HoldableButtonLAF::Brake:
            {
                parameterID = "brake";
                break;
            }
        }
    }
    
    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = getLocalBounds().toFloat();
        bounds.reduce(5, 5);
        float x = bounds.getX();
        float y = bounds.getY();
        float width = bounds.getWidth();

        switch(lookAndFeel){
            case HoldableButtonLAF::NudgeForward:
            {
                drawNudge(g, x, y, width, 3.14f);
                break;
            }
            case HoldableButtonLAF::NudgeBackward:
            {
                drawNudge(g, x, y, width, 0.0f);
                break;
            }
            case HoldableButtonLAF::Brake:
            {
                drawBrake(g, x, y, width);
                break;
            }
        }
    }
    
    void drawNudge(juce::Graphics &g, float x, float y, float size, float rotation)
    {
        juce::Path graphicPath;
        for (int i = 0; i < 5; i++)
        {
            float sizeIncr = (size/5) * (i * (1.0f - lpgValue));
            juce::Point<float> topPoint = { x + sizeIncr, y };
            juce::Point<float> botPoint = { x + sizeIncr, y + size };
            
            graphicPath.startNewSubPath(topPoint);
            graphicPath.lineTo(botPoint);
            
        }
        graphicPath.applyTransform(juce::AffineTransform::rotation(rotation, x + size/2, y + size/2));
        g.setColour(Colors::graphicBlackAlt);
        auto strokeType = juce::PathStrokeType(2.0f, juce::PathStrokeType::JointStyle::curved);
        strokeType.setEndStyle(juce::PathStrokeType::EndCapStyle::rounded);
        g.strokePath(graphicPath, strokeType);

    }

    void drawBrake(juce::Graphics &g, float x, float y, float size)
    {
        juce::Path topPath, bottomPath;
        float topHeight = size * 0.8f * lpgValue;
        float bottomHeight = size * 0.8f * (1.0f - lpgValue);

        topPath.addRoundedRectangle(x + size * 0.05f,
                                    y,
                                    size * 0.9f,
                                    topHeight,
                                    size * 0.1f);
        
        bottomPath.addRoundedRectangle(x + size * 0.05f,
                                       (y + size * 0.2f) + topHeight,
                                       size * 0.9f,
                                       bottomHeight,
                                       size * 0.1f);
        
        g.setColour(Colors::graphicBlackAlt);
        g.fillPath(topPath);
        g.strokePath(topPath, juce::PathStrokeType(2.0f));
        g.fillPath(bottomPath);
        g.strokePath(bottomPath, juce::PathStrokeType(2.0f));
    }
    
    void mouseDown(const juce::MouseEvent& e) override
    {
        buttonState = true;
            if (auto* param = audioProcessor.params->apvts.getParameter(parameterID))
                param->setValueNotifyingHost(1.0f);
    }
    
    void mouseUp(const juce::MouseEvent& e) override
    {
        buttonState = false;
        if (auto* param = audioProcessor.params->apvts.getParameter(parameterID))
            param->setValueNotifyingHost(0.0f);
    }
    
    float getButtonState()
    {
        return buttonState ? 1.0f : 0.0f;
    }
    
private:
    void timerCallback() override
    {
        slew.triggerEnvelope(getButtonState());
        lpgValue = slew.generateEnvelope();
        repaint();
    }
    
    
    juce::String parameterID;
    bool buttonState;
    LowPassGate slew;
    float lpgValue = 0.0f;
    
    HoldableButtonLAF lookAndFeel;
    TingeAudioProcessor& audioProcessor;
};
