#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "DSP.h"
#include "Graphics.h"
#include "UserInterfaceLookAndFeel.h"

/*
class GlobalControlLayout : public juce::Component, juce::Button::Listener, juce::Timer
{
public:
    GlobalControlLayout(TingeAudioProcessor &p) : audioProcessor(p)
    {
        addAndMakeVisible(nudgeStrengthSlider);
        nudgeStrengthSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        nudgeStrengthSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        nudgeStrengthSlider.setLookAndFeel(&nudgeStrengthLAF);
        nudgeStrengthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params->apvts, "nudgeStrength", nudgeStrengthSlider);

        setLabel(nudgeLabel, "Nudge", juce::Justification::centred);
        
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

        brakeButton.setBounds(x, y, height, height);
        
      // nudgeLabel.setBounds(x + width * 0.05f, y + height * 0.75, width * 0.15f, height * 0.15f);
        nudgeBackwardButton.setBounds(x + width * 0.05f, y, width * 0.05f, height * 0.75f);
        nudgeForwardButton.setBounds(x + width * 0.15f, y, width * 0.05f, height * 0.75f);
        nudgeStrengthSlider.setBounds(x + width * 0.2f, y, width * 0.05f, height * 0.75f);

    }
    
    void buttonClicked(juce::Button* b) override {}
    
    void buttonStateChanged(juce::Button* b) override
    {
        if (b == &nudgeForwardButton) {
            
            float nudgeValue = b->isDown() ? 1.0f : 0.0f;
            nudgeForwardLPG.triggerEnvelope(b->isDown());
            audioProcessor.params->apvts.getParameter("nudgeForward")->setValueNotifyingHost(nudgeValue);
            
        } else if (b == &nudgeBackwardButton)
        {
            float nudgeValue = b->isDown() ? 1.0f : 0.0f;
            nudgeBackwardLPG.triggerEnvelope(b->isDown());
            audioProcessor.params->apvts.getParameter("nudgeBackward")->setValueNotifyingHost(nudgeValue);
            
        } else if (b == &brakeButton)
        {
            float nudgeValue = b->isDown() ? 1.0f : 0.0f;
            audioProcessor.params->apvts.getParameter("brake")->setValueNotifyingHost(nudgeValue);
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
    void setLabel(juce::Label& label, juce::String labelText, juce::Justification justification)
    {
        juce::FontOptions font(12.0f, juce::Font::plain);
        addAndMakeVisible(label);
        label.setText(labelText, juce::dontSendNotification);
        label.setColour(juce::Label::textColourId, juce::Colour(80, 80, 80));
        label.setJustificationType(justification);
        label.setFont(font);
    }

    ButtonGraphics nudgeForwardLAF { 0 }, nudgeBackwardLAF { 1 }, brakeLAF { 2 };
    DialGraphics nudgeStrengthLAF { 2 };
    
    juce::Label nudgeLabel, brakeLabel, scaleLabel;
    
    juce::Slider nudgeStrengthSlider, brakeStrengthSlider;
    juce::TextButton nudgeBackwardButton, nudgeForwardButton, brakeButton, syncButton;
    std::array<juce::TextButton, 7> rateScaleButton;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
    nudgeStrengthAttachment,
    brakeStrengthAttachment;
    
    LowPassGate nudgeForwardLPG,
    nudgeBackwardLPG,
    brakeLPG;

    TingeAudioProcessor& audioProcessor;
};


*/
