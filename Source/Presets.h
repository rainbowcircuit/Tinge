/*
  ==============================================================================

    Presets.h
    Created: 20 Aug 2025 2:49:38pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class PresetManager : juce::ValueTree::Listener
{
public:
    static const juce::File defaultDirectory;
    static const juce::String extension;
    static const juce::String presetNameProperty;

    PresetManager(juce::AudioProcessorValueTreeState& apvts);
    ~PresetManager();
    
    void loadPreset(const juce::String& presetName);
    void savePreset(const juce::String& presetName);
    int loadNextPreset();
    int loadPreviousPreset();
    juce::StringArray getAllPreset() const;
    juce::String getCurrentPreset() const;
    
private:
    void valueTreeRedirected(juce::ValueTree &treeWhichHasBeenChanged) override;
    
    juce::AudioProcessorValueTreeState& apvts;
    juce::Value currentPreset;
};


