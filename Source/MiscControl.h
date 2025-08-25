/*
  ==============================================================================

    MiscControl.h
    Created: 20 Aug 2025 11:13:13am
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookAndFeel.h"
#include "GraphicsHelper.h"


enum class GlobalControlsLAF { NudgeForward, NudgeBackward, Brake, Reset };

class GlobalControlsLookAndFeel : public juce::LookAndFeel_V4, DrawHelper
{
public:
    GlobalControlsLookAndFeel(GlobalControlsLAF l) : lookAndFeel(l) {}
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    
    //==============================================================================
    void drawNudge(juce::Graphics &g, float x, float y, float size, bool drawNudgeBack);
    void drawBrake(juce::Graphics &g, float x, float y, float width, float height);
    void drawReset(juce::Graphics &g, float x, float y, float size);
    void drawTriangle(juce::Graphics& g, float size, juce::Point<float> endCoords, float rotation);
    
    //==============================================================================
    void setLPGValue(float lpgValue);

public:
    float lpgValue;
    GlobalControlsLAF lookAndFeel;
};


class GlobalControlsLayout : public juce::Component, juce::Button::Listener, juce::Timer, GraphicsHelper
{
public:
    GlobalControlsLayout(TingeAudioProcessor &p);
    ~GlobalControlsLayout();
    //==============================================================================

    void paint(juce::Graphics& g) override {}
    void resized() override;
    void buttonClicked(juce::Button* b) override {}
    void buttonStateChanged(juce::Button* b) override;

private:
    void timerCallback() override;

    LowPassGate nudgeForwardSlew, nudgeBackwardSlew, brakeSlew, resetSlew;
    
    GlobalControlsLookAndFeel
    nudgeForwardLAF { GlobalControlsLAF::NudgeForward },
    nudgeBackwardLAF { GlobalControlsLAF::NudgeBackward },
    brakeLAF { GlobalControlsLAF::Brake },
    resetLAF { GlobalControlsLAF::Reset };

    juce::Label nudgeLabel, brakeLabel, resetLabel;
    
    juce::Slider nudgeStrengthSlider, brakeStrengthSlider;
    juce::TextButton nudgeBackwardButton, nudgeForwardButton, brakeButton, resetButton;
    
    TingeAudioProcessor& audioProcessor;
};

class GlobalModeControlsLayoutpublic : public juce::Component, juce::Button::Listener, GraphicsHelper
{
public:
    
private:
    
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
    nudgeStrengthTextSlider,
    brakeStrengthTextSlider;
    
    juce::Label
    resetModeLabel;
    
    juce::TextButton
    nextButton,
    prevButton;
};
