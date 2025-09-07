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

enum class SpinnerControlsLAF { RateDisplay, Rate, Sync, Phase, Opacity, Shape };

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
    
//    void drawRoundSlider(juce::Graphics &g, float x, float y, float size, float position);
    void drawRateDisplay(juce::Graphics& g, float x, float y, float width, float height, float pos);
    void drawRate(juce::Graphics& g, float x, float y, float width, float height, float pos);

 //   void drawRatio(juce::Graphics& g, float x, float y, float size, float index);
    
    void drawSync(juce::Graphics &g, float x, float y, float width, float height, float pos);

    void drawPhase(juce::Graphics& g, float x, float y, float width, float height, float phase,  bool isBackground);

    
    void drawShape(juce::Graphics &g, float x, float y, float width, float height, float pos);
    
    void drawOpacity(juce::Graphics &g, float x, float y, float width, float height, float pos);
    
    void drawTextEditorOutline(juce::Graphics& g, int width, int height, juce::TextEditor& texteditor) override {}
    void fillTextEditorBackground(juce::Graphics& g, int width, int height, juce::TextEditor& textEditor) override {}
    juce::Label* createSliderTextBox(juce::Slider& slider) override { return createTextBox(slider); }


private:
    float lineWidth = 1.5f;
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
    SpinnerControlsLookAndFeel
    rateDisplayLAF { SpinnerControlsLAF::RateDisplay },
    rateLAF { SpinnerControlsLAF::Rate },
    phaseLAF { SpinnerControlsLAF::Phase },
    shapeLAF { SpinnerControlsLAF::Shape },
    opacityLAF { SpinnerControlsLAF::Opacity };
        
    juce::Slider rateFreeDisplaySlider,
    rateSyncDisplaySlider,
    rateFreeDialSlider,
    rateSyncDialSlider,
    phaseDialSlider,
    shapeDialSlider,
    opacityDialSlider;
    
    std::unique_ptr<EditableTextBoxSlider>
    rateFreeTextSlider,
    rateSyncTextSlider;
    
    juce::Label rateLabel,
    syncLabel,
    phaseLabel,
    shapeLabel,
    opacityLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
    rateFreeAttachment,
    rateSyncAttachment,
    phaseAttachment,
    shapeAttachment,
    opacityAttachment;

    juce::ToggleButton rateModeButton;
    
    TingeAudioProcessor& audioProcessor;
    
};
