#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "DSP.h"
#include "Graphics.h"
#include "UserInterfaceLookAndFeel.h"

class GlobalControlLayout : public juce::Component, juce::Button::Listener, juce::Timer
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
        
        nudgeForwardLPG.setSampleRate(60.0f);
        nudgeForwardLPG.setEnvelopeSlew(4400.0f, 4400.0f);

        nudgeBackwardLPG.setSampleRate(60.0f);

        brakeLPG.setSampleRate(60.0f);
        
        startTimerHz(60);
        
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
            nudgeForwardLPG.triggerEnvelope(b->isDown());
            audioProcessor.apvts.getParameter("nudgeForward")->setValueNotifyingHost(nudgeValue);
            
        } else if (b == &nudgeBackwardButton)
        {
            float nudgeValue = b->isDown() ? 1.0f : 0.0f;
            nudgeBackwardLPG.triggerEnvelope(b->isDown());
            audioProcessor.apvts.getParameter("nudgeBackward")->setValueNotifyingHost(nudgeValue);
            
        } else if (b == &brakeButton)
        {
            float nudgeValue = b->isDown() ? 1.0f : 0.0f;
            audioProcessor.apvts.getParameter("brake")->setValueNotifyingHost(nudgeValue);
        }
    }

    void timerCallback() override
    {
        float nudgeForward = nudgeForwardLPG.generateEnvelope();
        nudgeForwardLAF.setLPGValue(nudgeForward);
        nudgeForwardButton.repaint();
        
        float nudgeBackward = nudgeBackwardLPG.generateEnvelope();
        nudgeBackwardLAF.setLPGValue(nudgeBackward);
        nudgeBackwardButton.repaint();

    }
    
    void setLPGStrength(float nudgeStrength)
    {
        float riseScaled = nudgeStrength * 4.0f;
        float fallScaled = nudgeStrength;
        nudgeForwardLPG.setEnvelopeSlew(riseScaled, fallScaled);
        nudgeBackwardLPG.setEnvelopeSlew(riseScaled, fallScaled);
        brakeLPG.setEnvelopeSlew(riseScaled, fallScaled);
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
    
    LowPassGate nudgeForwardLPG, nudgeBackwardLPG, brakeLPG;
    juce::TextButton nudgeBackwardButton, nudgeForwardButton, brakeButton, syncButton;
    
    TingeAudioProcessor& audioProcessor;
};

class RotationLayout : public juce::Component, juce::Button::Listener
{
public:
    RotationLayout(TingeAudioProcessor &p, int index) : audioProcessor(p)
    {
        this->index = index;
        
        bool rateMode = audioProcessor.apvts.getRawParameterValue("rateMode" + juce::String(index))->load();
        setSlider(rateFreeSlider, juce::Slider::NoTextBox, rateLAF);
        rateFreeSlider.setVisible(!rateMode);
        juce::String rateFreeID = "rateFree" + juce::String(index);
        rateFreeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, rateFreeID, rateFreeSlider);
        
        setSlider(rateSyncSlider, juce::Slider::NoTextBox, rateLAF);
        rateSyncSlider.setVisible(rateMode);
        juce::String rateSyncID = "rateSync" + juce::String(index);
        rateSyncAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, rateSyncID, rateSyncSlider);

        
        // clean this up later
        setLabel(rateLabel, "Rate", juce::Justification::centredLeft);
        setLabel(ratioLabel, "Ratio", juce::Justification::centredLeft);
        setLabel(phaseLabel, "Phase", juce::Justification::centredLeft);
        setLabel(opacityLabel, "Opacity", juce::Justification::centredLeft);

        rateFreeTextSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, "rateFree" + juce::String(index), "Hz");
        addAndMakeVisible(*rateFreeTextSlider);
        rateFreeTextSlider->setFontSize(12.0f);

        rateSyncTextSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, "rateSync" + juce::String(index), "");
        addAndMakeVisible(*rateSyncTextSlider);
        rateSyncTextSlider->setFontSize(12.0f);
        rateSyncTextSlider->setSliderMode(true, false);
        rateSyncTextSlider->setVisible(false);


        setSlider(ratioSlider, juce::Slider::NoTextBox, ratioLAF);
        juce::String ratioID = "division" + juce::String(index);
        ratioAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, ratioID, ratioSlider);
        
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
        
        for (int i = 0; i < 24; i++)
        {
            addAndMakeVisible(colorPickerButton[i]);
            colorPickerButton[i].addListener(this);
            colorPickerButton[i].setRadioGroupId(0);
            colorPickerButton[i].setToggleable(true);
            colorPickerButton[i].setClickingTogglesState(true);
            colorPickerButton[i].setLookAndFeel(&colorButtonLAF[i]);
            colorButtonLAF[i].setColorButtonIndex(i);
            colorPickerButton[i].repaint();
        }
    }
    
    ~RotationLayout()
    {
        rateModeButton.removeListener(this);
        for (int i = 0; i < 24; i++)
        {
            colorPickerButton[i].removeListener(this);
        }
    }
    
    void paint(juce::Graphics& g) override {}
    
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
        
        phaseLabel.setBounds(x + width * 0.55f, height * 0.4f, width * 0.25f, height * 0.15f);
        phaseTextSlider->setBounds(x + width * 0.75f, height * 0.4f, width * 0.25f, height * 0.15f);


        ratioSlider.setBounds(x, y + height * 0.6f, width * 0.5, height * 0.1f);
        ratioLabel.setBounds(x, y + height * 0.65f, width * 0.5, height * 0.15f);
        
        
        for (int i = 0; i < 24; i++)
        {
            float xPos = x + (width/12) * (i % 12);
            float yPos = (y + height * 0.8f) + (height * 0.1f) * (i / 12);
            colorPickerButton[i].setBounds(xPos, yPos, width * 0.1f, height * 0.1f);
        }
        
        opacityLabel.setBounds(x + width * 0.5f, y + height * 0.65f, width * 0.5, height * 0.15f);
        opacityTextSlider->setBounds(x + width * 0.75f, height * 0.65f, width * 0.25f, height * 0.15f);
    }
    
    void buttonClicked(juce::Button* b) override
    {
        if (b == &stateButton){
            bool state = stateButton.getToggleState();
            juce::String stateID = "state" + juce::String(index);
            audioProcessor.apvts.getParameter(stateID)->setValueNotifyingHost(state);
            
        } else if (b == &rateModeButton){
            
            juce::String rateModeID = "rateMode" + juce::String(index);
            bool rateMode = rateModeButton.getToggleState();
            float rateModeFloat = rateMode ? 0.0f : 1.0f;
            audioProcessor.apvts.getParameter(rateModeID)->setValueNotifyingHost(rateModeFloat);
            
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
        
        for (int i = 0; i < 24; i++){
            if (b == &colorPickerButton[i]){
                juce::String colorIndexID = "colorIndex" + juce::String(index);
                auto range = audioProcessor.apvts.getParameter(colorIndexID)->getNormalisableRange();
                float colorIndex = range.convertTo0to1(i);
                audioProcessor.apvts.getParameter(colorIndexID)->setValueNotifyingHost(colorIndex);
            }
        }
    }

    
    void buttonStateChanged(juce::Button* b) override {}
    
    bool isMouseOver(juce::Point<int> mouse)
    {
        auto bounds = getBounds();        
        return bounds.contains(mouse);
    }
    
    void setSlider(juce::Slider& slider, juce::Slider::TextEntryBoxPosition textBoxPosition, DialGraphics& lookAndFeel)
    {
        // slider
        addAndMakeVisible(slider);
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(textBoxPosition, false, 50, 20);
        slider.setLookAndFeel(&lookAndFeel);
    }
    
    void setLabel(juce::Label& label, juce::String labelText, juce::Justification justification)
    {
        juce::FontOptions font(12.0f, juce::Font::plain);
        addAndMakeVisible(label);
        label.setText(labelText, juce::dontSendNotification);
        label.setColour(juce::Label::textColourId, juce::Colour(80, 80, 80));
        label.setJustificationType(justification);
        label.setFont(font);
    }

private:
    int index;
    bool state;
    DialGraphics rateLAF { 0 }, ratioLAF { 1 };
    std::array<ColorButtonGraphics, 24> colorButtonLAF;
    std::array<juce::TextButton, 24> colorPickerButton;
    
    juce::Slider rateFreeSlider,
    rateSyncSlider,
    ratioSlider;

    std::unique_ptr<EditableTextBoxSlider>
    rateFreeTextSlider,
    rateSyncTextSlider,
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
