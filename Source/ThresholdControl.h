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


class TingeAudioProcessor;

enum class ThresholdLAF { Max, Phase, Overlay, Equidistant, Fill, Harmonic, Random, Sequential, Fibonacci};
class ThresholdLookAndFeel : public juce::LookAndFeel_V4, DrawHelper
{
public:
    ThresholdLookAndFeel(ThresholdLAF l) : lookAndFeel(l) {}
    
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider) override;
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    
    
    void drawThresholdPhase(juce::Graphics &g, float x, float y, float size, float position);
    void drawThresholdMax(juce::Graphics &g, float x, float y, float size, float position);
    
    
    void drawOverlapBackground(juce::Graphics &g, float x, float y, float size, bool state, int c1, int c2);
    
    void drawEquidistant(juce::Graphics &g, float x, float y, float size, bool state);
    void drawFill(juce::Graphics &g, float x, float y, float size, bool state);
    void drawHarmonic(juce::Graphics &g, float x, float y, float size, bool state);
    void drawRandom(juce::Graphics &g, float x, float y, float size, bool state);
    void drawSequential(juce::Graphics &g, float x, float y, float size, bool state);
    void drawFibonacci(juce::Graphics &g, float x, float y, float size, bool state);
    void drawFibonacciCube(juce::Graphics &g, float x, float y, float size, float rotation, float alpha);
    
    float lineWidth;
    ThresholdLAF lookAndFeel;
};

class ThresholdGraphics : public juce::Component, Interaction
{
    void paint(juce::Graphics& g) override {}
    void resized() override {}

};

class ThresholdLayout : public juce::Component, public juce::Button::Listener, public juce::Timer, public GraphicsHelper
{
public:
    ThresholdLayout(TingeAudioProcessor &p);
    ~ThresholdLayout() {}
    void paint(juce::Graphics& g) override
    {
     //   g.setColour(Colors::graphicWhiteAlt);
     //   g.fillAll();
    }
    
    void resized() override;
    void buttonClicked(juce::Button* b) override {}
    void buttonStateChanged(juce::Button* b) override {}

private:
    void timerCallback() override {}
    
  //  SpinnerControlsLookAndFeel dialLAF { SpinnerControlsLAF::RoundDial };
    
    ThresholdLookAndFeel
    maxLAF { ThresholdLAF::Max },
    phaseLAF { ThresholdLAF::Phase },
    equiDistantLAF { ThresholdLAF::Equidistant },
    fillLAF { ThresholdLAF::Fill },
    harmonicLAF { ThresholdLAF::Harmonic },
    randomLAF { ThresholdLAF::Random },
    sequentialLAF { ThresholdLAF::Sequential },
    fibonacciButtonLAF { ThresholdLAF::Fibonacci };

    
    juce::Slider rateFreeSlider,
    thresholdPhaseDial,
    thresholdMaxDial,
    thresholdStretchDial,
    thresholdSkewDial;
    
   // std::unique_ptr<EditableTextBoxSlider>

    
    juce::Label
    thresholdModeLabel,
    thresholdPhaseLabel,
    thresholdMaxLabel;
    
    juce::TextButton equiDistantButton,
    fillButton,
    harmonicButton,
    randomButton,
    sequentialButton,
    fibonacciButton;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
    thresholdPhaseAttachment,
    thresholdMaxAttachment;

};

