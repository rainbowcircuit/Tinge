/*
  ==============================================================================

    ThresholdControl.h
    Created: 31 Aug 2025 9:57:38am
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "EditableTextSlider.h"
#include "SpinnerControl.h"
#include "GraphicsHelper.h"
#include "InteractionLogic.h"
#include "LookAndFeel.h"
#include "ThresholdGraphics.h"

class TingeAudioProcessor;

enum class ThresholdLAF { Max, Phase, ValueSlew, Equidistant, Fill, Harmonic, Random, Fibonacci};
class ThresholdLookAndFeel : public juce::LookAndFeel_V4, DrawHelper
{
public:
    ThresholdLookAndFeel(ThresholdLAF l) : lookAndFeel(l) {}

    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider) override;
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    
    //==============================================================================
    void drawThresholdPhase(juce::Graphics &g, float x, float y, float size, float phase, bool isBackGround);
    void drawThresholdMax(juce::Graphics &g, float x, float y, float width, float height, float position);
    void drawValueSlew(juce::Graphics &g, float x, float y, float width, float height, float position);
    
    //==============================================================================
    void drawEquidistant(juce::Graphics &g, float x, float y, float size, bool state);
    void drawFill(juce::Graphics &g, float x, float y, float size, bool state);
    void drawHarmonic(juce::Graphics &g, float x, float y, float size, bool state);
    void drawRandom(juce::Graphics &g, float x, float y, float size, bool state);
    void drawFibonacci(juce::Graphics &g, float x, float y, float size, bool state);
    void drawFibonacciCube(juce::Graphics &g, float x, float y, float size, float rotation, float alpha);
    
private:
    float lineWidth;
    juce::Colour graphicGrey;
    ThresholdLAF lookAndFeel;
};

class ThresholdLayout : public juce::Component, public juce::Button::Listener, public juce::Timer, public GraphicsHelper
{
public:
    ThresholdLayout(TingeAudioProcessor &p);
    ~ThresholdLayout();
    void paint(juce::Graphics& g) override
    {
     //   g.setColour(Colors::graphicWhiteAlt);
     //   g.fillAll();
    }
    
    void resized() override;
    void buttonClicked(juce::Button* b) override;
    void buttonStateChanged(juce::Button* b) override {}

    ThresholdGraphics graphics;
private:
    void timerCallback() override {}
    
    
    ThresholdLookAndFeel
    maxLAF { ThresholdLAF::Max },
    phaseLAF { ThresholdLAF::Phase },
    valueSlewLAF { ThresholdLAF::ValueSlew };

    
    juce::Slider
    thresholdPhaseDial,
    thresholdMaxDial,
    valueSlewDial;
    
    std::unique_ptr<EditableTextBoxSlider>
    thresholdMaxTextSlider;

    
    juce::Label
    thresholdModeLabel,
    thresholdPhaseLabel,
    thresholdMaxLabel,
    valueSlewLabel;
    
    
    
    juce::TextButton equiDistantButton,
    fillButton,
    harmonicButton,
    randomButton,
    fibonacciButton;
    
    std::array<juce::TextButton, 5> thresholdModeButton;
    
    std::array<ThresholdLookAndFeel, 5> thresholdModeLAF = {
        ThresholdLookAndFeel { ThresholdLAF::Equidistant },
        ThresholdLookAndFeel { ThresholdLAF::Fill },
        ThresholdLookAndFeel { ThresholdLAF::Harmonic },
        ThresholdLookAndFeel { ThresholdLAF::Random },
        ThresholdLookAndFeel { ThresholdLAF::Fibonacci }
    };
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
    thresholdPhaseAttachment,
    thresholdMaxAttachment,
    valueSlewAttachment;
    
    TingeAudioProcessor& audioProcessor;
};

