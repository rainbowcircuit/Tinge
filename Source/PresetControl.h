/*
  ==============================================================================

    PresetControl.h
    Created: 22 Aug 2025 2:28:47pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GraphicsHelper.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Presets.h"

class TingeAudioProcessor;
class TingeAudioProcessorEditor;


enum class PresetControlsLAF { Save, Next, Previous, ComboBox };

class PresetControlsLookAndFeel : public juce::LookAndFeel_V4
{
public:
    PresetControlsLookAndFeel(PresetControlsLAF l) : lookAndFeel(l)
    {
        setColour(juce::ComboBox::textColourId, juce::Colours::transparentBlack);
    }
    
    //==============================================================================
    void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void setMarginAndFont(int margin, int fontSize);
    //==============================================================================
    void drawSaveButton(juce::Graphics& g, float x, float y, float size);
    void drawArrowButton(juce::Graphics& g, float x, float y, float size, bool isLeftArrow);

    //==============================================================================
    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& comboBox) override;
    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area, bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu, const juce::String& text, const juce::String& shortcutKeyText, const juce::Drawable* icon, const juce::Colour* textColour) override;
    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override;
    void drawResizableFrame(juce::Graphics& g, int w, int h, const juce::BorderSize<int>& b) override {}

    
private:
    PresetControlsLAF lookAndFeel;
    int margin = 5;
    int fontSize = 12;
};

class PresetControlsLayout : public juce::Component, juce::ComboBox::Listener, juce::Button::Listener, GraphicsHelper
{
public:
    PresetControlsLayout(TingeAudioProcessor& p, TingeAudioProcessorEditor& e, juce::AudioProcessorValueTreeState& apvts);
    ~PresetControlsLayout();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void comboBoxChanged(juce::ComboBox *comboBoxThatHasChanged) override;
    void buttonClicked(juce::Button* buttonClicked) override;
    void setEditorSizeIndex(int editorSizeIndex);
    void loadPresetList();
    
private:
    PresetControlsLookAndFeel saveLAF { PresetControlsLAF::Save },
    nextLAF { PresetControlsLAF::Next },
    prevLAF { PresetControlsLAF::Previous },
    comboBoxLAF { PresetControlsLAF::ComboBox };
    
    juce::TextButton saveButton, nextButton, prevButton;
    juce::ComboBox presetComboBox, editorSizeComboBox;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rateAttachment;
    std::unique_ptr<juce::FileChooser> fileChooser;

    PresetManager presetManager;
    
    TingeAudioProcessorEditor& editor;
    TingeAudioProcessor& audioProcessor;
};
