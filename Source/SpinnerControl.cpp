/*
  ==============================================================================

    SpinnerControl.cpp
    Created: 19 Aug 2025 9:05:52pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#include "SpinnerControl.h"

void SpinnerControlsLookAndFeel::setIndex(int index)
{
    spinnerIndex = index;
}

void SpinnerControlsLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider)
{
    auto bounds = slider.getLocalBounds().toFloat();
    bounds.reduce(5, 5);
    float xPos = bounds.getX();
    float yPos = bounds.getY();
    float size = bounds.getWidth();
    float graphicWidth = bounds.getWidth();
    float graphicHeight = bounds.getHeight();
    
    switch(lookAndFeel){
        case SpinnerControlsLAF::RateDisplay:
        {
            drawRateDisplay(g, xPos, yPos, graphicWidth, graphicHeight, sliderPosProportional);
            break;
        }

        case SpinnerControlsLAF::Rate:
        {
            drawRate(g, xPos, yPos + graphicHeight * 0.25f, graphicWidth, graphicHeight * 0.5f, sliderPosProportional);
            break;
        }
        case SpinnerControlsLAF::Phase:
        {
            drawPhase(g, xPos, yPos, graphicWidth, graphicHeight, 0.0f, false);
            drawPhase(g, xPos, yPos, graphicWidth, graphicHeight, sliderPosProportional, true);
            break;
        }
            
        case SpinnerControlsLAF::Curve:
        {
            drawCurve(g, xPos, yPos, graphicWidth, graphicHeight, sliderPosProportional);
            break;
        }
        case SpinnerControlsLAF::Opacity:
        {
            drawOpacity(g, xPos, yPos, graphicWidth, graphicHeight, sliderPosProportional);
            break;
        }
    }
}

void SpinnerControlsLookAndFeel::drawRateDisplay(juce::Graphics& g, float x, float y, float width, float height, float pos)
{
    int domainResolution = 128;
    float xIncr = width/domainResolution;
    juce::Path sinMainPath, offsetLPath, offsetRPath;
    
    float twopi = juce::MathConstants<float>::twoPi;
    sinMainPath.startNewSubPath(x, y + height/2);
    offsetLPath.startNewSubPath(x, y + height/2);
    offsetRPath.startNewSubPath(x, y + height/2);

    float posBipolar = (std::abs(pos - 0.5f) * 2);
    
    for (int i = 1; i < domainResolution; i++)
    {
        int hannCenterPoint = domainResolution * pos;
        int hannWidth = (1.0f - posBipolar) * 64 + 64;
        int hannStartPoint = juce::jlimit(0, domainResolution, hannCenterPoint - hannWidth/2);
        int hannEndPoint = juce::jlimit(0, domainResolution, hannCenterPoint +
                                        hannWidth/2);

        int hannLength = hannEndPoint - hannStartPoint;

        float hann = 0.0f;
        float phase = 0.0f;
        
        if (i > hannStartPoint && i < hannEndPoint){
            int j = i - hannStartPoint;
            hann = 0.5f * (1.0f - std::cos(twopi * j/hannLength));
            
            float frequency = (std::abs(pos - 0.5f) * 2) * 1.0f + 1.0f;
            phase = (twopi * std::pow(frequency, 2) * j) / hannLength;
        }
        
        float sin = std::sin(phase) * hann;
        float sinOffset1 = std::sin(phase * (std::abs(pos - 0.5f) + 0.5f)) * hann;
        float sinOffset2 = std::sin(phase * -(std::abs(pos - 0.5f) + 0.5f)) * hann;

        float heightOffset = (height * 0.25f * (1.0f - posBipolar)) + (height * 0.25f);
        
        juce::Point<float> sinCoords = { x + xIncr * i, (y + height/2) + sin * heightOffset };
        juce::Point<float> offsetLCoords = { sinCoords.x, (y + height/2) + sinOffset1 * heightOffset * 0.85f };
        juce::Point<float> offsetRCoords = { sinCoords.x, (y + height/2) + sinOffset2 * heightOffset * 0.85f };

        
        sinMainPath.lineTo(sinCoords.x, sinCoords.y);
        sinMainPath = sinMainPath.createPathWithRoundedCorners(10);
        
        offsetLPath.lineTo(offsetLCoords.x, offsetLCoords.y);
        offsetLPath = offsetLPath.createPathWithRoundedCorners(10);

        offsetRPath.lineTo(offsetRCoords.x, offsetRCoords.y);
        offsetRPath = offsetRPath.createPathWithRoundedCorners(10);

    }
    offsetRPath.lineTo(x + width, y + height/2);
    offsetRPath.lineTo(x + width, y + height);
    offsetRPath.lineTo(x, y + height);
    offsetRPath.closeSubPath();
    offsetRPath = offsetRPath.createPathWithRoundedCorners(height * 0.15f);
    
    auto color = Palette::darkenColors(Colors::primaryColor[(spinnerIndex + 1) % 3], Colors::primaryColor[spinnerIndex]);
    g.setColour(color.withAlpha((float)1.0f));
    g.fillPath(offsetRPath);

    
    sinMainPath.lineTo(x + width, y + height/2);
    sinMainPath.lineTo(x + width, y + height);
    sinMainPath.lineTo(x, y + height);
    sinMainPath.closeSubPath();
    sinMainPath = sinMainPath.createPathWithRoundedCorners(height * 0.15f);

    auto color2 = Palette::interpolateColors(color, Colors::primaryColor[spinnerIndex], 0.5f);
    g.setColour(color2.withAlpha((float)1.0f));
    g.fillPath(sinMainPath);
    
    offsetLPath.lineTo(x + width, y + height/2);
    offsetLPath.lineTo(x + width, y + height);
    offsetLPath.lineTo(x, y + height);
    offsetLPath.closeSubPath();
    offsetLPath = offsetLPath.createPathWithRoundedCorners(height * 0.15f);

    g.setColour(Colors::primaryColor[spinnerIndex]);
    g.fillPath(offsetLPath);

}

void SpinnerControlsLookAndFeel::drawRate(juce::Graphics& g, float x, float y, float width, float height, float pos)
{
    juce::Path frontEndPath, frontPath, backPath;
    juce::Point<float> centerCoords = { x + width/2, y + height/2 };
    
    float pi = juce::MathConstants<float>::pi;
    float radius = height * 0.75f;
    float startPoint = -1.57f;
    float dialPoint = startPoint + (pos * pi);

    juce::Path outlinePath, valuePath;
    
    valuePath.addCentredArc(x + width/2,
                            y + height,
                            radius,
                            radius,
                            0.0f, 0.0f, dialPoint, true);
    g.setColour(Colors::primaryColor[spinnerIndex]);
    g.strokePath(valuePath, juce::PathStrokeType(2.0f));

}

void SpinnerControlsLookAndFeel::drawPhase(juce::Graphics& g, float x, float y, float width, float height, float phase, bool isBackground)
{
    juce::Path graphicPath;
    graphicPath.startNewSubPath(x, y + height/2);
    
    float twopi = juce::MathConstants<float>::twoPi;
    int domainResolution = 128;
    float widthIncrement = width/domainResolution;
    float phaseScale = domainResolution/twopi;
    phase = (phase * 2.0f - 1.0f) * twopi;

    float freq = 2.0f;
    
    float sinStart = std::sin((0/phaseScale) * freq + phase);
    graphicPath.startNewSubPath(x + widthIncrement * 0, (y + height/2) + (height * sinStart/2));

    for (int i = 1; i < domainResolution; i++)
    {
        float sin = std::sin((i/phaseScale) * freq + phase);
        graphicPath.lineTo(x + widthIncrement * i, (y + height/2) + (height * sin/2));
    }

    graphicPath = graphicPath.createPathWithRoundedCorners(4.0f);
    
    auto color = isBackground ? Colors::graphicGrey : Colors::primaryColor[spinnerIndex];
    g.setColour(color);
    g.strokePath(graphicPath, juce::PathStrokeType(1.5f));
}

void SpinnerControlsLookAndFeel::drawCurve(juce::Graphics &g, float x, float y, float width, float height, float pos)
{
    juce::Path trianglePath, linePath;
    
    float triangleSize = width * 0.2;
    trianglePath.addTriangle(x + width/2,
                             y + height/2,
                             (x + width/2) - triangleSize/2,
                             (y + height/2) + triangleSize,
                             (x + width/2) + triangleSize/2,
                             (y + height/2) + triangleSize);
    
    trianglePath = trianglePath.createPathWithRoundedCorners(width * 0.05f);
    g.setColour(Colors::graphicGrey);
    g.fillPath(trianglePath);
    

    linePath.startNewSubPath(x + width * 0.1f, y + height * 0.3);
    linePath.lineTo(x + width * 0.9f, y + height * 0.3);
    float radius = height * 0.2f;
    linePath.addCentredArc(x + width/2,
                    y + height * 0.3,
                    radius,
                    radius,
                    0.0f,
                    1.57f,
                    4.71f,
                    true);


    float rotation = (pos - 0.5f) * (juce::MathConstants<float>::pi/4);
    linePath.applyTransform(juce::AffineTransform::rotation(rotation,
                                                            x + width/2,
                                                            y + height * 0.3));
    g.setColour(Colors::primaryColor[spinnerIndex]);
    g.fillPath(linePath);
    g.strokePath(linePath, juce::PathStrokeType(1.5f));


}

void SpinnerControlsLookAndFeel::drawOpacity(juce::Graphics &g, float x, float y, float width, float height, float pos)
{
    float radius = height * 0.5f;
    float offset = (0.75f - pos * 0.65f) * radius;

    juce::Path leftCircle, rightCircle;
    juce::Point centerCoords = { x + width/2, y + height/2 };
    
    leftCircle.addCentredArc(centerCoords.x - offset,
                             centerCoords.y,
                             radius,
                             radius,
                             0.0f,
                             0.0f,
                             6.28f,
                             true);
    
    rightCircle.addCentredArc(centerCoords.x + offset,
                              centerCoords.y,
                              radius,
                              radius,
                              0.0f,
                              0.0f,
                              6.28f,
                              true);
    
    g.setColour(Colors::primaryColor[spinnerIndex]);
    g.fillPath(leftCircle);
    
    g.setColour(Colors::graphicGrey);
    g.fillPath(rightCircle);
    
    auto multipliedColor = Palette::multiplyColors(Colors::primaryColor[spinnerIndex],
                                            Colors::primaryColor[spinnerIndex]);

    drawDoubleOverlap(g, leftCircle, rightCircle, multipliedColor, false);

}


    SpinnerLayout::SpinnerLayout(TingeAudioProcessor &p, int index) : audioProcessor(p)
    {
        this->index = index;
        
        bool rateMode = audioProcessor.params->rateMode[index]->get();
        addAndMakeVisible(rateModeButton);
        rateModeButton.setToggleable(true);
        rateModeButton.setClickingTogglesState(true);
        rateModeButton.addListener(this);
        
        rateDisplayLAF.setIndex(index);
        setSlider(*this, rateFreeDisplaySlider, rateDisplayLAF);
        rateFreeDisplaySlider.setVisible(rateMode);
        setSlider(*this, rateSyncDisplaySlider, rateDisplayLAF);
        rateSyncDisplaySlider.setVisible(!rateMode);

        juce::String rateFreeID = "rateFree" + juce::String(index);
        rateFreeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params->apvts, rateFreeID, rateFreeDisplaySlider);
        
        juce::String rateSyncID = "rateSync" + juce::String(index);
        rateSyncAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params->apvts, rateSyncID, rateSyncDisplaySlider);

        // rate
        setLabel(*this, rateLabel, "Rate", juce::Justification::left);
        setLabel(*this, rateModeLabel, "Sync/Free", juce::Justification::left);

        // rate text slider
        rateFreeTextSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, rateFreeID);
        addAndMakeVisible(*rateFreeTextSlider);
        rateFreeTextSlider->setFontSize(12.0f);
        rateFreeTextSlider->setUnitStyle(UnitStyle::Hertz);
        rateFreeTextSlider->setNumDecimals(2);
        rateFreeTextSlider->setJustification(juce::Justification::left);
        rateFreeTextSlider->setVisible(rateMode);
        
        rateSyncTextSlider = std::make_unique<EditableTextBoxSlider>(audioProcessor, rateSyncID);
        addAndMakeVisible(*rateSyncTextSlider);
        rateSyncTextSlider->setFontSize(12.0f);
        rateSyncTextSlider->setUnitStyle(UnitStyle::Sync);
        rateSyncTextSlider->setJustification(juce::Justification::left);
        rateSyncTextSlider->setVisible(!rateMode);
        
        

        // phase
        setLabel(*this, phaseLabel, "Phase", juce::Justification::centred);
        phaseLAF.setIndex(index);
        setSlider(*this, phaseDialSlider, phaseLAF);
        juce::String phaseID = "phase" + juce::String(index);
        phaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params->apvts, phaseID, phaseDialSlider);

        
        // opacity
        setLabel(*this, opacityLabel, "Opacity", juce::Justification::centred);
        opacityLAF.setIndex(index);
        setSlider(*this, opacityDialSlider, opacityLAF);
        juce::String opacityID = "opacity" + juce::String(index);
        opacityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params->apvts, opacityID, opacityDialSlider);

        // shape
        setLabel(*this, curveLabel, "Curve", juce::Justification::centred);
        curveLAF.setIndex(index);
        setSlider(*this, curveDialSlider, curveLAF);
        juce::String curveID = "curve" + juce::String(index);
        curveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params->apvts, curveID, curveDialSlider);

    }
    
    SpinnerLayout::~SpinnerLayout()
    {
        rateModeButton.removeListener(this);
    }
    
void SpinnerLayout::paint(juce::Graphics& g) {}

void SpinnerLayout::resized()
{
    auto bounds = getLocalBounds().toFloat();
    float margin = bounds.getWidth() * 0.05f;
    bounds.reduce(margin, margin);
    float x = bounds.getX();
    float y = bounds.getY();
    float width = bounds.getWidth();
    float height = bounds.getHeight();
    
    float rowLabelY = y + height * 0.5f;
    float rowHeight = getFontSize() * 1.5f;

    // rate display
    rateFreeDisplaySlider.setBounds(x,
                                    y,
                                    width,
                                    height * 0.5f);
    
    rateSyncDisplaySlider.setBounds(x,
                                    y,
                                    width,
                                    height * 0.5f);

    // rate text
    rateLabel.setBounds(x,
                        rowLabelY,
                        width * 0.225f,
                        rowHeight);

    rateFreeTextSlider->setBounds(x + width * 0.15f,
                                  rowLabelY,
                                  width * 0.3f,
                                  rowHeight);
    
    
    rateSyncTextSlider->setBounds(x + width * 0.18f,
                                  rowLabelY,
                                  width * 0.3f,
                                  rowHeight);
    
    rateModeLabel.setBounds(x + width * 0.55f,
                             rowLabelY,
                             width * 0.3f,
                             rowHeight);

    rateModeButton.setBounds(x + width * 0.85f,
                             rowLabelY,
                             width * 0.15f,
                             rowHeight);

    
    
    rowLabelY = y + height * 0.65f;
    float rowSliderY = y + height * 0.775f;

    // phase
    phaseLabel.setBounds(x,
                           rowLabelY,
                           width * 0.275f,
                           rowHeight);
    phaseLabel.setFont(juce::FontOptions(getFontSize()));
    phaseDialSlider.setBounds(x,
                                rowSliderY,
                                 width * 0.275f,
                                 height * 0.25f);
    
    phaseDialSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0.0f, 0.0f);

    // opacity
    opacityLabel.setBounds(x + width * 0.3375f,
                           rowLabelY,
                           width * 0.325f,
                           rowHeight);
    opacityLabel.setFont(juce::FontOptions(getFontSize()));

    opacityDialSlider.setBounds(x + width * 0.3375f,
                                rowSliderY,
                                 width * 0.325f,
                                 height * 0.25f);
    opacityDialSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0.0f, 0.0f);
    
    // curve
    curveLabel.setBounds(x + width * 0.6625f,
                           rowLabelY,
                           width * 0.325f,
                           rowHeight);
    curveLabel.setFont(juce::FontOptions(getFontSize()));

    curveDialSlider.setBounds(x + width * 0.6625f,
                                rowSliderY,
                                 width * 0.325f,
                                 height * 0.25f);
    curveDialSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0.0f, 0.0f);

}

void SpinnerLayout::buttonClicked(juce::Button* b)
{
        
     if (b == &rateModeButton){
        juce::String rateModeID = "rateMode" + juce::String(index);
        bool rateMode = rateModeButton.getToggleState();
        float rateModeFloat = rateMode ? 0.0f : 1.0f;
        audioProcessor.params->apvts.getParameter(rateModeID)->setValueNotifyingHost(rateModeFloat);
         
        rateFreeDisplaySlider.setVisible(!rateMode);
        rateFreeTextSlider->setVisible(!rateMode);
        rateSyncDisplaySlider.setVisible(rateMode);
        rateSyncTextSlider->setVisible(rateMode);
    }
}

bool SpinnerLayout::isMouseOver(juce::Point<int> mouse)
{
    auto bounds = getBounds();
    return bounds.contains(mouse);
}

void SpinnerLayout::timerCallback()
{
}
