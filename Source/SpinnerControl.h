/*
  ==============================================================================

    SpinnerControl.h
    Created: 19 Aug 2025 9:05:52pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookAndFeel.h"
#include "Graphics.h"
#include "EditableTextSlider.h"
#include "GraphicsHelper.h"

enum class SpinnerControlsLAF { Rate, rateMode, Phase, Opacity, Curve };

class SpinnerControlsLookAndFeel : public juce::LookAndFeel_V4, DrawHelper, GraphicsHelper
{
public:
    SpinnerControlsLookAndFeel(SpinnerControlsLAF l) : lookAndFeel(l)
    {
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        setColour(juce::Slider::textBoxTextColourId, Colors::graphicBlack);
    }
    void setIndex(int index);
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider) override;
    
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void drawRate(juce::Graphics& g, float x, float y, float width, float height, float pos);
    void drawRateMode(juce::Graphics &g, float x, float y, float size, bool state);
    void drawPhase(juce::Graphics& g, float x, float y, float width, float height, float phase,  bool isBackground);
    void drawCurve(juce::Graphics &g, float x, float y, float width, float height, float pos);
    void drawOpacity(juce::Graphics &g, float x, float y, float width, float height, float pos);
    
    void drawTextEditorOutline(juce::Graphics& g, int width, int height, juce::TextEditor& texteditor) override {}
    void fillTextEditorBackground(juce::Graphics& g, int width, int height, juce::TextEditor& textEditor) override {}
    juce::Label* createSliderTextBox(juce::Slider& slider) override { return createTextBox(slider); }


private:
    float lineWidth = 1.5f;
    int spinnerIndex;
    SpinnerControlsLAF lookAndFeel;
    
    juce::Colour primaryColor, graphicGrey;
    
};

class SpinnerLayout : public juce::Component, public juce::Button::Listener, public juce::Timer, public GraphicsHelper
{
public:
    SpinnerLayout(TingeAudioProcessor &p, int index);
    ~SpinnerLayout();
    void paint(juce::Graphics& g) override;
    
    void resized() override;
    void buttonClicked(juce::Button* b) override;
    void buttonStateChanged(juce::Button* b) override {}
    
    bool isMouseOver(juce::Point<int> mouse);

private:
    void timerCallback() override;
    
    int index;
    int hover;
    SpinnerControlsLookAndFeel
    rateLAF { SpinnerControlsLAF::Rate },
    rateModeLAF { SpinnerControlsLAF::rateMode },
    phaseLAF { SpinnerControlsLAF::Phase },
    curveLAF { SpinnerControlsLAF::Curve },
    opacityLAF { SpinnerControlsLAF::Opacity };
        
    juce::Slider rateFreeDisplaySlider,
    rateSyncDisplaySlider,
    rateFreeDialSlider,
    rateSyncDialSlider,
    phaseDialSlider,
    curveDialSlider,
    opacityDialSlider;
    
    std::unique_ptr<EditableTextBoxSlider>
    rateFreeTextSlider,
    rateSyncTextSlider;
    
    juce::Label rateLabel,
    rateModeLabel,
    phaseLabel,
    curveLabel,
    opacityLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
    rateFreeAttachment,
    rateSyncAttachment,
    phaseAttachment,
    opacityAttachment,
    curveAttachment;

    juce::ToggleButton rateModeButton;
    
    TingeAudioProcessor& audioProcessor;
    
};
