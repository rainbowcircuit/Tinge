/*
  ==============================================================================

    PresetControl.cpp
    Created: 22 Aug 2025 2:28:47pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#include "PresetControl.h"

void PresetControlsLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();
  //  bounds.reduce(margin, margin);
    
    float x = bounds.getX();
    float y = bounds.getY();
    float width = bounds.getWidth();
    float height = bounds.getHeight();

    switch (lookAndFeel) {
        case PresetControlsLAF::Save:
            drawSaveButton(g, (x + width/2) - (height/2), y, height);
            break;
            
        case PresetControlsLAF::Next:
            drawArrowButton(g, (x + width/2) - (height/2), y, height, true);
            break;
            
        case PresetControlsLAF::Previous:
            drawArrowButton(g, (x + width/2) - (height/2), y, height, false);
            break;
            
        default:
            break;
    }
}

void PresetControlsLookAndFeel::setMarginAndFont(int margin, int fontSize)
{
    this->margin = margin;
    this->fontSize = fontSize;
}

void PresetControlsLookAndFeel::drawSaveButton(juce::Graphics& g, float x, float y, float size)
{
    float graphicMargin = size * 0.3f;
    float graphicSize = size * 0.4f;
    x = x + graphicMargin;
    y = y + graphicMargin;

    juce::Point<float> topLeft { x, y };
    juce::Point<float> slopeStart { x + graphicSize * 0.75f, y };
    juce::Point<float> slopeEnd { x + graphicSize, y + graphicSize * 0.25f};
    juce::Point<float> botRight { x + graphicSize, y + graphicSize };
    juce::Point<float> botLeft { x, y + graphicSize };

    juce::Path bodyPath;
    bodyPath.startNewSubPath(topLeft);
    bodyPath.lineTo(slopeStart);
    bodyPath.lineTo(slopeEnd);
    bodyPath.lineTo(botRight);
    bodyPath.lineTo(botLeft);
    bodyPath.closeSubPath();
    bodyPath = bodyPath.createPathWithRoundedCorners(1.0f);
    g.setColour(Colors::graphicBlack);
    g.fillPath(bodyPath);
}


void PresetControlsLookAndFeel::drawArrowButton(juce::Graphics& g, float x, float y, float size, bool isLeftArrow)
{
    float graphicMargin = size * 0.35f;
    float graphicSize = size * 0.3f;
    x = x + graphicMargin;
    y = y + graphicMargin;

    // coordinates
    juce::Point<float> topLeft { x, y };
    juce::Point<float> botLeft { x, y + graphicSize };
    juce::Point<float> middleRight { x + graphicSize, y + graphicSize/2 };
    juce::Point<float> topRight { x + graphicSize, y };
    juce::Point<float> middleLeft { x, y + graphicSize/2 };
    juce::Point<float> botRight { x + graphicSize, y + graphicSize };

    // drawing
    juce::Path arrowPath;
    if (isLeftArrow){
        arrowPath.startNewSubPath(topLeft);
        arrowPath.lineTo(botLeft);
        arrowPath.lineTo(middleRight);
        arrowPath.closeSubPath();
    } else {
        arrowPath.startNewSubPath(topRight);
        arrowPath.lineTo(botRight);
        arrowPath.lineTo(middleLeft);
        arrowPath.closeSubPath();
    }

    arrowPath = arrowPath.createPathWithRoundedCorners(1.0f);
    g.setColour(Colors::graphicBlack);
    g.fillPath(arrowPath);
}

void PresetControlsLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& comboBox)
{
    auto bounds = juce::Rectangle<int>(width, height).toFloat();
    bounds.reduce(margin, margin);
    juce::Path comboBoxPath, trianglePath, buttonPath;
    
    comboBoxPath.addRoundedRectangle(bounds, margin/2);
  //  g.setColour(Colors::backgroundFill);
   // g.fillPath(comboBoxPath);
    
    g.setFont(fontSize);
    g.setColour(Colors::graphicBlack);
    g.drawText(comboBox.getText(), bounds, juce::Justification::centred, false);
}

void PresetControlsLookAndFeel::drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area, bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu, const juce::String& text, const juce::String& shortcutKeyText, const juce::Drawable* icon, const juce::Colour* textColour)
{
    juce::Path menuPath;
    menuPath.addRectangle(area);
    
    juce::Colour menuColor;
    menuColor = isHighlighted ? Colors::backgroundFillAlt : Colors::backgroundFill;
    g.setColour(menuColor);
    g.fillPath(menuPath);
    
    g.setColour(Colors::graphicBlack);
    g.setFont(fontSize);
    g.drawText(text, area, juce::Justification::centred, false);
}

void PresetControlsLookAndFeel::drawPopupMenuBackground(juce::Graphics& g, int width, int height)
{
    juce::Path menuPath;
    menuPath.addRectangle(0, 0, width, height);
    g.setColour(Colors::backgroundFillAlt);
    g.fillPath(menuPath);
}



PresetControlsLayout::PresetControlsLayout(TingeAudioProcessor& p, TingeAudioProcessorEditor& e, juce::AudioProcessorValueTreeState& apvts) : presetManager(apvts), editor(e), audioProcessor(p)
{
    addAndMakeVisible(saveButton);
    saveButton.addListener(this);
    saveButton.setLookAndFeel(&saveLAF);

    addAndMakeVisible(nextButton);
    nextButton.addListener(this);
    nextButton.setLookAndFeel(&nextLAF);

    addAndMakeVisible(prevButton);
    prevButton.addListener(this);
    prevButton.setLookAndFeel(&prevLAF);
    
    addAndMakeVisible(presetComboBox);
    presetComboBox.addListener(this);
    presetComboBox.setLookAndFeel(&comboBoxLAF);
    loadPresetList();
    
    addAndMakeVisible(editorSizeComboBox);
    editorSizeComboBox.addListener(this);
    editorSizeComboBox.setLookAndFeel(&comboBoxLAF);
    editorSizeComboBox.addItem("Small", 1);
    editorSizeComboBox.addItem("Medium", 2);
    editorSizeComboBox.addItem("Large", 3);

}

PresetControlsLayout::~PresetControlsLayout()
{
    saveButton.removeListener(this);
    nextButton.removeListener(this);
    prevButton.removeListener(this);
    presetComboBox.removeListener(this);
}

void PresetControlsLayout::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    float x = bounds.getX();
    float y = bounds.getY();
    float height = bounds.getHeight();
    float width = bounds.getWidth();

    juce::Path linePath;
    linePath.startNewSubPath(x, y + height/2);
    linePath.lineTo(x + width, y + height/2);
    
    linePath.startNewSubPath(x + width * 0.333, y + height/2);
    linePath.lineTo(x + width * 0.333, y + height);

    linePath.startNewSubPath(x + width * 0.666, y + height/2);
    linePath.lineTo(x + width * 0.666, y + height);

    g.setColour(juce::Colour(180, 180, 180));
    g.strokePath(linePath, juce::PathStrokeType(1.0f));
}


void PresetControlsLayout::resized()
{
    auto bounds = getLocalBounds().toFloat();
    bounds.reduce(getMargin(), getMargin());
    float x = bounds.getX();
    float y = bounds.getY();
    float height = bounds.getHeight();
    float width = bounds.getWidth();

    saveLAF.setMarginAndFont(getMargin(), getFontSize());
    saveButton.setBounds(x,
                         y + height * 0.5f,
                         width * 0.333f,
                         height * 0.5f);
    saveButton.repaint();
    
    prevLAF.setMarginAndFont(getMargin(), getFontSize());
    prevButton.setBounds(x + width * 0.333f,
                         y + height * 0.5f,
                         width * 0.333f,
                         height * 0.5f);
    prevButton.repaint();

    nextLAF.setMarginAndFont(getMargin(), getFontSize());
    nextButton.setBounds(x + width * 0.666f,
                         y + height * 0.5f,
                         width * 0.333f,
                         height * 0.5f);
    nextButton.repaint();



    presetComboBox.setBounds(x,
                             y,
                             width,
                             height * 0.5f);
    presetComboBox.repaint();
    /*
    editorSizeComboBox.setBounds(x + width - (height * 3.0f),
                                 bounds.getY(),
                                 height * 3.0f,
                                 height);
    */
    comboBoxLAF.setMarginAndFont(getMargin(), getFontSize());
    editorSizeComboBox.repaint();
}

void PresetControlsLayout::comboBoxChanged(juce::ComboBox *comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &presetComboBox)
        presetManager.loadPreset(presetComboBox.getItemText(presetComboBox.getSelectedItemIndex()));
    
 //   if (comboBoxThatHasChanged == &editorSizeComboBox)
 //       editor.changeEditorSize(editorSizeComboBox.getSelectedItemIndex());
}

void PresetControlsLayout::buttonClicked(juce::Button* buttonClicked)
{
    if (buttonClicked == &saveButton){
        fileChooser = std::make_unique<juce::FileChooser>(
            "Enter Preset Name",
            presetManager.defaultDirectory,
            "*." + presetManager.extension);
        
        fileChooser->launchAsync(juce::FileBrowserComponent::saveMode, [&](const juce::FileChooser& chooser)
        {
            const auto resultFile = chooser.getResult();
            presetManager.savePreset(resultFile.getFileNameWithoutExtension());
            loadPresetList();
        });

    } else if (buttonClicked == &nextButton){
        presetManager.loadNextPreset();
        loadPresetList();
        
    } else if (buttonClicked == &prevButton){
        presetManager.loadPreviousPreset();
        loadPresetList();
    }
}

void PresetControlsLayout::setEditorSizeIndex(int editorSizeIndex)
{
    std::array<juce::String, 3> editorSize = { "Small", "Medium", "Large" };
    editorSizeComboBox.setText(editorSize[editorSizeIndex]);
}


void PresetControlsLayout::loadPresetList()
{
    presetComboBox.clear(juce::dontSendNotification);
    const auto allPresets = presetManager.getAllPreset();
    const auto currentPreset = presetManager.getCurrentPreset();
    presetComboBox.addItemList(allPresets, 1);
    presetComboBox.setTitle(currentPreset);
    presetComboBox.setSelectedItemIndex(allPresets.indexOf(currentPreset), juce::dontSendNotification);
}

