#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Graphics.h"
#include "UserInterfaceLookAndFeel.h"

class GlobalControlLayout : public juce::Component, juce::Button::Listener
{
public:
    GlobalControlLayout(TingeAudioProcessor &p) : audioProcessor(p)
    {
        addAndMakeVisible(nudgeForwardButton);
        nudgeForwardButton.setLookAndFeel(&nudgeForwardLAF);
        nudgeForwardButton.addListener(this);
        
        addAndMakeVisible(nudgeBackwardButton);
        nudgeBackwardButton.setLookAndFeel(&nudgeBackwardLAF);
        nudgeBackwardButton.addListener(this);
        
        addAndMakeVisible(brakeButton);
        brakeButton.setLookAndFeel(&brakeLAF);

        brakeButton.addListener(this);

    }
    
    void resized() override
    {
        auto bounds = getLocalBounds().toFloat();
        float x = bounds.getX();
        float y = bounds.getY();
        float width = bounds.getWidth();
        float height = bounds.getHeight();

        nudgeBackwardButton.setBounds(x + 50, y, height, height);
        nudgeForwardButton.setBounds(x + 100, y, height, height);
        brakeButton.setBounds(x, y, height, height);

    }
    
    void buttonClicked(juce::Button* b) override { }
    
    void buttonStateChanged(juce::Button* b) override
    {
        if (b == &nudgeForwardButton) {
            
            float nudgeValue = b->isDown() ? 1.0f : 0.0f;
            audioProcessor.apvts.getParameter("globalNudgeForward")->setValueNotifyingHost(nudgeValue);
            
        } else if (b == &nudgeBackwardButton)
        {
            float nudgeValue = b->isDown() ? 1.0f : 0.0f;
            audioProcessor.apvts.getParameter("globalNudgeBackward")->setValueNotifyingHost(nudgeValue);
            
        } else if (b == &brakeButton)
        {
            float nudgeValue = b->isDown() ? 1.0f : 0.0f;
            audioProcessor.apvts.getParameter("globalBrake")->setValueNotifyingHost(nudgeValue);
        }
    }

    
private:
    void setSlider(juce::Label& label, juce::Slider& slider, juce::Slider::TextEntryBoxPosition textBoxPosition, juce::String labelText, DialGraphics& lookAndFeel)
    {
        // initialize label
        addAndMakeVisible(label);
        label.setText(labelText, juce::dontSendNotification);
        label.setColour(juce::Label::textColourId, juce::Colour(80, 80, 80));
        label.setJustificationType(juce::Justification::centred);
        juce::FontOptions font(12.0f, juce::Font::plain);
        label.setFont(font);

        // slider
        addAndMakeVisible(slider);
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(textBoxPosition, false, 50, 20);
        slider.setLookAndFeel(&lookAndFeel);
    }
    
    
    ButtonGraphics nudgeForwardLAF { 0 }, nudgeBackwardLAF { 1 }, brakeLAF { 2 };
    
    juce::TextButton nudgeBackwardButton, nudgeForwardButton, brakeButton, syncButton;
    
    TingeAudioProcessor& audioProcessor;
};

class RotationLayout : public juce::Component, juce::Button::Listener
{
public:
    RotationLayout(TingeAudioProcessor &p, int index) : audioProcessor(p)
    {
        this->index = index;
        
        setSlider(rateFreeSlider, juce::Slider::NoTextBox, rateLAF);
        juce::String rateFreeID = "rateFree" + juce::String(index);
        rateFreeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, rateFreeID, rateFreeSlider);
        
        setSlider(rateSyncSlider, juce::Slider::NoTextBox, rateLAF);
        juce::String rateSyncID = "rateSync" + juce::String(index);
        rateSyncAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, rateSyncID, rateSyncSlider);

        // clean this up later
        juce::FontOptions font(12.0f, juce::Font::plain);
        addAndMakeVisible(rateLabel);
        rateLabel.setText("Rate", juce::dontSendNotification);
        rateLabel.setColour(juce::Label::textColourId, juce::Colour(80, 80, 80));
        rateLabel.setJustificationType(juce::Justification::centredLeft);
        rateLabel.setFont(font);

        addAndMakeVisible(divisionLabel);
        divisionLabel.setText("Division", juce::dontSendNotification);
        divisionLabel.setColour(juce::Label::textColourId, juce::Colour(80, 80, 80));
        divisionLabel.setJustificationType(juce::Justification::centred);
        divisionLabel.setFont(font);

        addAndMakeVisible(phaseLabel);
        phaseLabel.setText("Phase", juce::dontSendNotification);
        phaseLabel.setColour(juce::Label::textColourId, juce::Colour(80, 80, 80));
        phaseLabel.setJustificationType(juce::Justification::centredLeft);
        phaseLabel.setFont(font);

        
        rateFreeTextSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, "rateFree" + juce::String(index), "Hz");
        addAndMakeVisible(*rateFreeTextSlider);
        rateFreeTextSlider->setFontSize(12.0f);

        rateSyncTextSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, "rateSync" + juce::String(index), "");
        addAndMakeVisible(*rateSyncTextSlider);
        rateSyncTextSlider->setFontSize(12.0f);
        rateSyncTextSlider->setSliderMode(true, false);


        setSlider(divisionSlider, juce::Slider::NoTextBox, divisionLAF);
        juce::String divisionID = "division" + juce::String(index);
        divisionAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, divisionID, divisionSlider);

        phaseTextSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, "phase" + juce::String(index), "%");
        addAndMakeVisible(*phaseTextSlider);
        phaseTextSlider->setFontSize(12.0f);

        opacityTextSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, "opacity" + juce::String(index), "");
        addAndMakeVisible(*opacityTextSlider);
        opacityTextSlider->setFontSize(12.0f);

        addAndMakeVisible(stateButton);
        stateButton.setToggleable(true);
        stateButton.setClickingTogglesState(true);
        stateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "state" + juce::String(index), stateButton);
        
        
        addAndMakeVisible(rateModeButton);
        rateModeButton.setToggleable(true);
        rateModeButton.setClickingTogglesState(true);
        rateModeButton.addListener(this);
        
    }
    
    ~RotationLayout()
    {
        rateModeButton.removeListener(this);

    }
    
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
     //   g.setColour(juce::Colour(100, 100, 100));
     //   g.fillRoundedRectangle(bounds, 2);
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds().toFloat();
        float x = bounds.getX();
        float y = bounds.getY();
        float width = bounds.getWidth();
        float height = bounds.getHeight();
        
        rateFreeSlider.setBounds(x, y, width, height * 0.4f);
        rateSyncSlider.setBounds(x, y, width, height * 0.4f);
        rateLabel.setBounds(x, height * 0.4f, width, height * 0.15f);

        rateFreeTextSlider->setBounds(x + width * 0.15f, y + height * 0.4f, width * 0.25f, height * 0.15f);
        rateSyncTextSlider->setBounds(x + width * 0.15f, y + height * 0.4f, width * 0.25f, height * 0.15f);
        
        rateModeButton.setBounds(x + width * 0.4f, y + height * 0.4f, height * 0.15f, height * 0.15f);
        
        phaseLabel.setBounds(x + width * 0.6f, height * 0.4f, width * 0.25f, height * 0.15f);
        phaseTextSlider->setBounds(x + width * 0.75f, height * 0.4f, width * 0.25f, height * 0.15f);


        
        divisionSlider.setBounds(x, y + height * 0.6f, width * 0.5, height * 0.3f);
        divisionLabel.setBounds(x, y + height * 0.85f, width * 0.5, height * 0.15f);

        opacityTextSlider->setBounds(x + width * 0.6f, height * 0.8f, width, height * 0.15f);
    }
    
    void buttonClicked(juce::Button* b) override { }
    
    void buttonStateChanged(juce::Button* b) override
    {
            
        if (b == &stateButton){
            bool state = stateButton.getToggleState();
            juce::String stateID = "state" + juce::String(index);
            audioProcessor.apvts.getParameter(stateID)->setValueNotifyingHost(state);
            
        } else if (b == &rateModeButton){
            bool rateMode = rateModeButton.getToggleState();
            if (rateMode)
            {
                rateFreeSlider.setVisible(true);
                rateSyncSlider.setVisible(false);
                rateFreeTextSlider->setVisible(true);
                rateSyncTextSlider->setVisible(false);
                
            } else {
                rateFreeSlider.setVisible(false);
                rateSyncSlider.setVisible(true);
                rateFreeTextSlider->setVisible(false);
                rateSyncTextSlider->setVisible(true);
            }
            
            juce::String stateID = "state" + juce::String(index);
            audioProcessor.apvts.getParameter(stateID)->setValueNotifyingHost(rateMode);
        }
    }
    
    void setSlider(juce::Slider& slider, juce::Slider::TextEntryBoxPosition textBoxPosition, DialGraphics& lookAndFeel)
    {
        // slider
        addAndMakeVisible(slider);
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(textBoxPosition, false, 50, 20);
        slider.setLookAndFeel(&lookAndFeel);
    }

private:
    int index;
    bool state;
    DialGraphics rateLAF { 0 }, divisionLAF { 1 } ;
    
    juce::Slider rateFreeSlider,
    rateSyncSlider,
    divisionSlider;

    std::unique_ptr<EditableTextBoxSlider>
    rateFreeTextSlider,
    rateSyncTextSlider,
    phaseTextSlider,
    opacityTextSlider;
    
    juce::Label rateLabel,
    divisionLabel,
    phaseLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
    rateFreeAttachment,
    rateSyncAttachment,
    divisionAttachment,
    phaseAttachment,
    opacityAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> stateAttachment;

    juce::TextButton nudgeBackwardButton, nudgeForwardButton;
    juce::ToggleButton stateButton, rateModeButton;
    TingeAudioProcessor& audioProcessor;
    
};

class ValueAssignmentLayout : public juce::Component
{
public:
    ValueAssignmentLayout(TingeAudioProcessor& p) : audioProcessor(p)
    {
        notesScaleSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, "noteScale", "");
        velocityScaleSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, "velocityScale", "");
        pressureScaleSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, "controlScale", "");
        addAndMakeVisible(*notesScaleSlider);
        addAndMakeVisible(*velocityScaleSlider);
        addAndMakeVisible(*pressureScaleSlider);

        addAndMakeVisible(keyComboBox);
        for (int i = 1; i < keyArray.size(); i++)
            keyComboBox.addItem(keyArray[i], i);
        
        addAndMakeVisible(scaleComboBox);
        for (int i = 1; i < scaleArray.size(); i++)
            scaleComboBox.addItem(scaleArray[i], i);

    }
    
    void paint(juce::Graphics &g) override {}
    
    void resized() override
    {
        auto bounds = getLocalBounds().toFloat();
        float x = bounds.getX();
        float y = bounds.getY();
        float width = bounds.getWidth();
        float height = bounds.getHeight();

        notesScaleSlider->setBounds(x + width * 0.5f, y + height * 0.1f, width * 0.5f, height * 0.1f);
        velocityScaleSlider->setBounds(x + width * 0.5f, y + height * 0.3f, width * 0.5f, height * 0.1f);
        pressureScaleSlider->setBounds(x + width * 0.5f, y + height * 0.8f, width * 0.5f, height * 0.1f);

        keyComboBox.setBounds(x + width * 0.5f, y + height * 0.3f, width * 0.5f, height * 0.15f);
        scaleComboBox.setBounds(x + width * 0.5f, y + height * 0.5f, width * 0.5f, height * 0.15f);

    }
    
private:
    std::unique_ptr<EditableTextBoxSlider> notesScaleSlider, velocityScaleSlider, pressureScaleSlider;

    juce::ComboBox keyComboBox, scaleComboBox;
    TingeAudioProcessor& audioProcessor;
    
    juce::StringArray scaleArray = { "Chromatic", "Major", "Major Pentatonic", "Lydian", "Mixolydian", "Phrygian Dominant", "Natural Minor", "Minor Pentatonic", "Harmonic Minor", "Melodic Minor", "Dorian", "Phrygian", "Locrian" };
    
    juce::StringArray keyArray = { "C", "C#/Db", "D", "D#/Eb", "E", "F", "F#/Gb", "G", "G#/Ab", "A", "A#/Bb", "B" };
};
