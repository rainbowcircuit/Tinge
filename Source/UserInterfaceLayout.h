
#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Graphics.h"


class RotationLayout : public juce::Component, juce::Button::Listener
{
public:
    RotationLayout(TingeAudioProcessor &p, int index) : audioProcessor(p)
    {
        this->index = index;
        
        setSlider(rateLabel, rateFreeSlider, juce::Slider::NoTextBox, "Rate", emptyGraphic);
        juce::String rateFreeID = "rateFree" + juce::String(index);
        rateFreeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, rateFreeID, rateFreeSlider);
        
        
        setSlider(divisionLabel, divisionSlider, juce::Slider::NoTextBox, "Division", divisionGraphic);
        juce::String divisionID = "division" + juce::String(index);
        divisionAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, divisionID, divisionSlider);

        setSlider(nudgeStrengthLabel, nudgeStrengthSlider, juce::Slider::NoTextBox, "Nudge Strength", emptyGraphic);
        
        setSlider(divisionLabel, opacitySlider, juce::Slider::NoTextBox, "Division", emptyGraphic);
        juce::String opacityID = "opacity" + juce::String(index);
        opacityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, opacityID, opacitySlider);

        
        addAndMakeVisible(nudgeBackwardButton);
        nudgeBackwardButton.addListener(this);
        addAndMakeVisible(nudgeForwardButton);
        nudgeForwardButton.addListener(this);
        
        addAndMakeVisible(stateButton);
        stateButton.setToggleable(true);
        stateButton.addListener(this);
        
    }
    
    ~RotationLayout()
    {
        nudgeBackwardButton.removeListener(this);
        nudgeForwardButton.removeListener(this);
    }
    
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        bounds.reduce(5, 5);
        g.setColour(juce::Colour(170, 170, 17050)); // for now
        g.fillRoundedRectangle(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), 10);
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds().toFloat();
        float x = bounds.getX();
        float y = bounds.getY();
        float width = bounds.getWidth();
        float height = bounds.getHeight();
        
        float sliderSize = height * 0.8f;
        rateFreeSlider.setBounds(x, y + 5, sliderSize, sliderSize);
        divisionSlider.setBounds(x + 90, y + 5, sliderSize, sliderSize);
        opacitySlider.setBounds(x + 320, y + 5, sliderSize, sliderSize);

        nudgeBackwardButton.setBounds(x + 200, y + 15, height/2, height/4);
        nudgeForwardButton.setBounds(x + 260, y + 15, height/2, height/4);
        stateButton.setBounds(x + 320, y + 15, height/2, height/4);
    }
    
    void buttonClicked(juce::Button* b) override { }
    
    void buttonStateChanged(juce::Button* b) override
    {
        if (b == &nudgeForwardButton) {
            
            float nudgeValue = b->isDown() ? 1.0f : 0.0f;
            juce::String nudgeForwardID = "nudgeForward" + juce::String(index);
            audioProcessor.apvts.getParameter(nudgeForwardID)->setValueNotifyingHost(nudgeValue);
            
        } else if (b == &nudgeBackwardButton)
        {
            float nudgeValue = b->isDown() ? 1.0f : 0.0f;
            juce::String nudgeBackwardID = "nudgeBackward" + juce::String(index);
            audioProcessor.apvts.getParameter(nudgeBackwardID)->setValueNotifyingHost(nudgeValue);
        } else if (b == &stateButton){
            bool state = stateButton.getToggleState();
            juce::String stateID = "state" + juce::String(index);
            audioProcessor.apvts.getParameter(stateID)->setValueNotifyingHost(state);
        }
    }
    
    void setSlider(juce::Label& label, juce::Slider& slider, juce::Slider::TextEntryBoxPosition textBoxPosition, juce::String labelText, DialGraphics& lookAndFeel)
    {
        // initialize label
        addAndMakeVisible(label);
        label.setText(labelText, juce::dontSendNotification);
        label.setColour(juce::Label::textColourId, juce::Colour(80, 80, 80));
        label.setJustificationType(juce::Justification::centred);
        label.setFont(12.0f);

        // slider
        addAndMakeVisible(slider);
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(textBoxPosition, false, 50, 20);
        slider.setLookAndFeel(&lookAndFeel);
    }

private:
    int index;
    bool state;
    DialGraphics emptyGraphic { 0 }, divisionGraphic { 1 };
    
    juce::Slider rateFreeSlider, rateSyncSlider, divisionSlider, nudgeStrengthSlider, opacitySlider;
    juce::Label rateLabel, divisionLabel, nudgeStrengthLabel, nudgeBackwardLabel, nudgeForwardLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
    rateFreeAttachment, rateSyncAttachment, divisionAttachment, opacityAttachment;

    juce::TextButton nudgeBackwardButton, nudgeForwardButton;
    juce::ToggleButton stateButton;
    TingeAudioProcessor& audioProcessor;
    
};
