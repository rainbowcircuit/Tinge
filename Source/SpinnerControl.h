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

enum class SpinnerControlsLAF { Rate, Ratio, Opacity };

class SpinnerControlsLookAndFeel : public juce::LookAndFeel_V4, DrawHelper
{
public:
    SpinnerControlsLookAndFeel(SpinnerControlsLAF l) : lookAndFeel(l) {}
    void setIndex(int index);
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider) override;
    
    void drawRate(juce::Graphics& g, float x, float y, float width, float height, float pos);
    void drawRatio(juce::Graphics& g, float x, float y, float size, float index);
    void drawOpacity(juce::Graphics& g, float x, float y, float width, float height, float pos);
    void drawCrescent(juce::Graphics& g, float x, float y, float size, float amount);


private:
    int spinnerIndex;
    SpinnerControlsLAF lookAndFeel;
    juce::Colour iconShade = juce::Colour(190, 190, 190);
    juce::Colour iconDisabled = juce::Colour(90, 90, 90);
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
    SpinnerControlsLookAndFeel rateLAF { SpinnerControlsLAF::Rate },
    ratioLAF { SpinnerControlsLAF::Ratio },
    opacityLAF { SpinnerControlsLAF::Opacity };
        
    juce::Slider rateFreeSlider,
    rateSyncSlider,
    ratioSlider,
    opacitySlider;

    std::unique_ptr<EditableTextBoxSlider>
    rateFreeTextSlider,
    rateSyncTextSlider,
    ratioTextSlider,
    phaseTextSlider,
    opacityTextSlider;
    
    juce::Label rateLabel,
    ratioLabel,
    phaseLabel,
    opacityLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
    rateFreeAttachment,
    rateSyncAttachment,
    ratioAttachment,
    phaseAttachment,
    colorPickerAttachment,
    opacityAttachment;


    juce::TextButton nudgeBackwardButton, nudgeForwardButton;
    juce::ToggleButton rateModeButton;
    TingeAudioProcessor& audioProcessor;
    
};
