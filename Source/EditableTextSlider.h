/*
  ==============================================================================

    EditableTextSlider.h
    Created: 19 Aug 2025 9:17:57pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class EditableTextBoxSlider : public juce::Component, juce::Timer, juce::AudioProcessorParameter::Listener, juce::AsyncUpdater, juce::Label::Listener
{
public:
    EditableTextBoxSlider(TingeAudioProcessor& p, juce::String parameterID, juce::String parameterSuffix) : audioProcessor(p)
    {
        this->parameterID = parameterID;
        this->parameterSuffix = parameterSuffix;
        
        addAndMakeVisible(textBox);
        textBox.setEditable(false, false, false);
        textBox.setInterceptsMouseClicks(false, false);
        
        // initialize displayed value
        auto value = audioProcessor.params->apvts.getRawParameterValue(parameterID)->load();
        juce::String formattedValue = juce::String(value, numDecimals) + parameterSuffix;
        textBox.setText(formattedValue, juce::dontSendNotification);
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
        
        l->setText(juce::String(valueLimited, numDecimals), juce::dontSendNotification);
        textBox.setInterceptsMouseClicks(false, false);
        
        float normalized = (valueLimited - rangeStart) / (rangeEnd - rangeStart);
        textValueToParamValue(normalized);
        repaint();
    }

    void textValueToParamValue(float value)
    {
        value = juce::jlimit(0.0f, 1.0f, value);
        audioProcessor.params->apvts.getParameter(parameterID)->setValueNotifyingHost(value);
    }
        
    void parameterValueChanged (int parameterIndex, float newValue) override
    {
        newValueAtomic.store(newValue);
        parameterIndexAtomic.store(parameterIndex);
        triggerAsyncUpdate();
    }
    
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override {}
    
    void handleAsyncUpdate() override
    {
        float newValue = newValueAtomic.load();
        int parameterIndex = parameterIndexAtomic.load();
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
            juce::String formattedValue = juce::String(scaledValue, numDecimals) + parameterSuffix;
            textBox.setText(formattedValue, juce::dontSendNotification);
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
    
    void timerCallback() override
    {
        auto bounds = getLocalBounds().toFloat();
        auto mouse = getMouseXYRelative().toFloat();
        if (bounds.contains(mouse))
        {
            setMouseCursor(juce::MouseCursor::UpDownResizeCursor);
        }
    }
    
private:
    float initialParamValue;
    float rangeStart, rangeEnd;
    std::atomic<float> newValueAtomic;
    std::atomic<int> parameterIndexAtomic;
    
    juce::Point<float> dragStartPoint;
    juce::Label textBox;
    int numDecimals = 1;
    juce::String parameterID, parameterSuffix = "";
    
    TingeAudioProcessor& audioProcessor;
};

