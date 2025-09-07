
#pragma once
#include <JuceHeader.h>
#include "LookAndFeel.h"

enum class UnitStyle { Int, Float, Time, Frequency, Decibel, Percent, Semitone, Custom };

class EditableTextBoxSlider : public juce::Component, juce::Timer, juce::AudioProcessorParameter::Listener, juce::AsyncUpdater, juce::Label::Listener
{
public:
    EditableTextBoxSlider(TingeAudioProcessor& p, juce::String parameterID) : audioProcessor(p)
    {
        this->parameterID = parameterID;
        
        addAndMakeVisible(textBox);
        textBox.setEditable(false, false, false);
        textBox.setInterceptsMouseClicks(false, false);
        textBox.setColour(juce::Label::outlineWhenEditingColourId, juce::Colours::transparentBlack);
        textBox.setColour(juce::Label::textColourId, Colors::graphicBlack);

        // initialize displayed value
        textBox.addListener(this);
        
        // initialize parameter ranges
        juce::NormalisableRange range = audioProcessor.params->apvts.getParameterRange(parameterID);
        rangeStart = range.start;
        rangeEnd = range.end;
        
        // add listener
        const auto params = audioProcessor.getParameters();
        for (auto param : params){
            param->addListener(this);
        }
        
        // start timer
        startTimerHz(30);
    }
    
    ~EditableTextBoxSlider()
    {
        const auto params = audioProcessor.getParameters();
        for (auto param : params){
            param->removeListener(this);
        }
    }
    
    void paint(juce::Graphics& g) override {}
    
    void resized() override {
        auto bounds = getLocalBounds();
        textBox.setBounds(bounds);
        
        auto value = audioProcessor.params->apvts.getRawParameterValue(parameterID)->load();
        juce::String formattedValue = formatValueWithUnit(value);
        textBox.setText(formattedValue, juce::dontSendNotification);
    }
        
    void mouseDown(const juce::MouseEvent& m) override
    {
        auto mousePoint = m.getPosition().toFloat();
        dragStartPoint.y = mousePoint.y;
        
        initialParamValue = audioProcessor.params->apvts.getParameter(parameterID)->getValue();
    }

    void mouseDrag(const juce::MouseEvent& m) override
    {
        auto mousePoint = m.getPosition().toFloat();
        float deltaY = mousePoint.y - dragStartPoint.y;
        
        float sensitivity = 0.005f;
        float newValue = juce::jlimit(0.0f, 1.0f, initialParamValue + (-deltaY * sensitivity));
        textValueToParamValue(newValue);
    }
    
    void mouseUp(const juce::MouseEvent& m) override
    {
        auto mousePoint = m.getPosition().toFloat();
        dragStartPoint.y = mousePoint.y;
    }
    
    void mouseDoubleClick(const juce::MouseEvent& m) override
    {
        textBox.setInterceptsMouseClicks(true, true);
        textBox.setEditable(true, true, false);
        textBox.showEditor();
        textBox.grabKeyboardFocus();
    }
    
    void labelTextChanged(juce::Label* l) override
    {
        auto value = l->getText().getFloatValue();
        float valueLimited = juce::jlimit(rangeStart, rangeEnd, value);
        
        l->setText(formatValueWithUnit(valueLimited), juce::dontSendNotification);
        textBox.setInterceptsMouseClicks(false, false);
        
        float normalized = (valueLimited - rangeStart) / (rangeEnd - rangeStart);
        textValueToParamValue(normalized);
        repaint();
    }
    
    void editorHidden(juce::Label *, juce::TextEditor &) override
    {
        textBox.setInterceptsMouseClicks(false, false);
    }
    
    void textValueToParamValue(float value)
    {
        value = juce::jlimit(0.0f, 1.0f, value);
        audioProcessor.params->apvts.getParameter(parameterID)->setValueNotifyingHost(value);
    }
        
    void parameterValueChanged (int parameterIndex, float newValue) override
    {
        const juce::SpinLock::ScopedLockType lock(pendingLock);
        pendingChanges.emplace_back(parameterIndex, newValue);
        triggerAsyncUpdate();
    }
    
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override {}
    
    void handleAsyncUpdate() override
    {
        std::vector<std::pair<int, float>> updatesCopy;

        {
            const juce::SpinLock::ScopedLockType lock(pendingLock);
            updatesCopy.swap(pendingChanges); // safely move all pending updates
        }

        for (const auto& [parameterIndex, newValue] : updatesCopy)
        {
            juce::String newParameterID;
            float scaledValue;
            
            if (auto* param = dynamic_cast<juce::AudioProcessorParameterWithID*>(audioProcessor.getParameters()[parameterIndex]))
            {
                if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(param))
                {
                    scaledValue = rangedParam->convertFrom0to1(newValue);
                    newParameterID = param->paramID;
                }
            }
            
            if (newParameterID == parameterID)
            {
                juce::String formattedValue = formatValueWithUnit(scaledValue);
                textBox.setText(formattedValue, juce::dontSendNotification);
            }
        }
    }

    void setFontSize(float size)
    {
        textBox.setFont(juce::FontOptions(size, juce::Font::plain));
    }
    
    void setNumDecimals(int numDecimals)
    {
        this->numDecimals = numDecimals;
    }
    
    void setSuffix(juce::String parameterSuffix)
    {
        this->parameterSuffix = parameterSuffix;
    }

    
    void timerCallback() override
    {
        auto bounds = getLocalBounds().toFloat();
        auto mouse = getMouseXYRelative().toFloat();
        if (bounds.contains(mouse))
        {
            setMouseCursor(juce::MouseCursor::UpDownResizeCursor);
        }
    }
    
    void setUnitStyle(UnitStyle style)
    {
        unitStyle = style; // Store the style for use in formatting
    }
    
    juce::String formatValueWithUnit(float value)
    {
        switch(unitStyle)
        {
            case UnitStyle::Int:
                return juce::String((int)std::round(value));
                
            case UnitStyle::Float:
                return juce::String(value, numDecimals);
                
            case UnitStyle::Time:
            {
                if (value >= 1000.0f)
                {
                    return juce::String(value / 1000.0f, numDecimals) + " s";
                }
                else
                {
                    return juce::String(value, numDecimals) + " ms";
                }
            }
            
            case UnitStyle::Frequency:
            {
                if (value >= 1000.0f)
                {
                    return juce::String(value / 1000.0f, numDecimals) + " kHz";
                }
                else
                {
                    return juce::String(value, numDecimals) + " Hz";
                }
            }
            
            case UnitStyle::Decibel:
                return juce::String(value, numDecimals) + " dB";
                
            case UnitStyle::Percent:
                return juce::String(value, numDecimals) + " %";
                
            case UnitStyle::Semitone:
                return juce::String(value, numDecimals) + " st";
                
            case UnitStyle::Custom:
                return juce::String(value, numDecimals) + parameterSuffix;

            default:
                return juce::String(value, numDecimals);
        }
    }

    
private:
    UnitStyle unitStyle = UnitStyle::Float;
    float initialParamValue;
    float rangeStart, rangeEnd;
    
    std::vector<std::pair<int, float>> pendingChanges;
    juce::SpinLock pendingLock;

    juce::Point<float> dragStartPoint;
    juce::Label textBox;
    int numDecimals = 1;
    juce::String parameterID, parameterSuffix = "";
    
    TingeAudioProcessor& audioProcessor;
};

