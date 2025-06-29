#include "Graphics.h"
SpinnerGraphics::SpinnerGraphics()
{
    rotationValue[0].state = true;
    rotationValue[1].state = true;
    rotationValue[2].state = true;
    
    rotationValue[0].color = juce::Colour(255, 0, 0);
    rotationValue[1].color = juce::Colour(0, 255, 0);
    rotationValue[2].color = juce::Colour(0, 0, 255);
    updater.addAnimator(isoEnterToggle);
    updater.addAnimator(isoExitToggle);
}

void SpinnerGraphics::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    float wheelWidth = bounds.getWidth() * 0.75f;
    float wheelMargin = bounds.getWidth() * 0.125f;
    float x = bounds.getX();
    float y = bounds.getY();
    
    float yOffset = yPosOffset * bounds.getHeight() * 2.0f;
    float sumYOffset = yPosOffset * bounds.getHeight() * 0.5f;

    juce::Path sumPath1 = wheelPath(g, 0, x + wheelMargin, y + 20 - sumYOffset, wheelWidth, wheelWidth);
    juce::Path sumPath2 = wheelPath(g, 1, x + wheelMargin, y + 20 - sumYOffset, wheelWidth, wheelWidth);
    juce::Path sumPath3 = wheelPath(g, 2, x + wheelMargin, y + 20 - sumYOffset, wheelWidth, wheelWidth);
    
    g.setColour(juce::Colour(255, 70, 70));
    if (rotationValue[0].state) { g.fillPath(sumPath1); }
    
    g.setColour(juce::Colour(70, 255, 150));
    if (rotationValue[1].state) { g.fillPath(sumPath2); }

    g.setColour(juce::Colour(100, 120, 255));
    if (rotationValue[2].state) { g.fillPath(sumPath3); }


    if (rotationValue[0].state && rotationValue[1].state) {
        drawDoubleOverlap(g, sumPath1, sumPath2, juce::Colour(254, 241, 0));
    }
    
    if (rotationValue[1].state && rotationValue[2].state) {
        drawDoubleOverlap(g, sumPath2, sumPath3, juce::Colour(62, 71, 205));
    }

    if (rotationValue[0].state && rotationValue[2].state) {
        drawDoubleOverlap(g, sumPath1, sumPath3, juce::Colour(237, 28, 37));
    }
    
    if (rotationValue[0].state && rotationValue[1].state && rotationValue[2].state) {
        drawTripleOverlap(g, sumPath1, sumPath2, sumPath3, juce::Colour(230, 230, 230));
    }
    drawThreshold(g, x + wheelMargin, y + 20 - sumYOffset, wheelWidth, wheelWidth);

    
    
    juce::Path path1 = wheelPath(g, 0, x + wheelMargin, y - 50 - yOffset, wheelWidth, wheelWidth);
    juce::Path path2 = wheelPath(g, 1, x + wheelMargin, y - 65 - yOffset, wheelWidth, wheelWidth);
    juce::Path path3 = wheelPath(g, 2, x + wheelMargin, y - 80 - yOffset, wheelWidth, wheelWidth);
    g.setColour(juce::Colour(255, 70, 70));
    if (rotationValue[0].state) { g.fillPath(path1); }
    
    g.setColour(juce::Colour(70, 255, 150));
    if (rotationValue[1].state) { g.fillPath(path2); }

    g.setColour(juce::Colour(100, 120, 255));
    if (rotationValue[2].state) { g.fillPath(path3); }
}

void SpinnerGraphics::drawDoubleOverlap(juce::Graphics& g, juce::Path& pathA, juce::Path& pathB, juce::Colour color)
{
    g.saveState();
    g.reduceClipRegion(pathA);
    g.reduceClipRegion(pathB);
    g.setColour(color);
    g.fillAll();
    g.restoreState();
}

void SpinnerGraphics::drawTripleOverlap(juce::Graphics& g, juce::Path& pathA, juce::Path& pathB, juce::Path& pathC, juce::Colour color)
{
    g.saveState();
    g.reduceClipRegion(pathA);
    g.reduceClipRegion(pathB);
    g.reduceClipRegion(pathC);
    g.setColour(color);
    g.fillAll();
    g.restoreState();
}

juce::Path SpinnerGraphics::wheelPath(juce::Graphics& g, int index, int x, int y, int width, int height)
{
    int division = rotationValue[index].division;
    if (division < 0) { division = 1; }
    
    float twopi = juce::MathConstants<float>::twoPi;
    float radius = width/2;
    
    juce::Path wheelPath;
    for(int i = 0; i < division * 2 - 1; i+= 2){
        
        float startAngle = rotationValue[index].angles[i] * twopi;
        float endAngle = rotationValue[index].angles[i + 1] * twopi;

        if (endAngle <= startAngle)
            endAngle += twopi;
        if (i % 2 == 0){
            
            wheelPath.startNewSubPath(x + width/2, y + height/(isometricSkew)); // center
            wheelPath.addCentredArc(x + width/2, y + height/(isometricSkew),
                                    radius, radius/(isometricSkew),
                                    0.0f, startAngle, endAngle);
            wheelPath.closeSubPath();
            wheelPath = wheelPath.createPathWithRoundedCorners(4);
        }
    }
    return wheelPath;
}

void SpinnerGraphics::drawThreshold(juce::Graphics& g, int x, int y, int width, int height)
{
    float twopi = juce::MathConstants<float>::twoPi;
    float radius = width * 0.45f;
    
    for (int i = 0; i < numThresholds; i++)
    {
        if (numThresholds > 0){
            float startCoords = (x + width/2) + std::cos(thresholdAngles[i] * twopi) * radius;
            float endCoords = (y + height/isometricSkew) + std::sin(thresholdAngles[i] * twopi) * radius/isometricSkew;
            
            juce::Path graphicPath;
            graphicPath.startNewSubPath(x + width/2, y + height/isometricSkew);
            graphicPath.lineTo(startCoords, endCoords);
            g.setColour(juce::Colour(40, 40, 40));
            
            
            
            bool bool0 = rotationValue[0].threshold[i] > 0.0f && rotationValue[0].state;
            bool bool1 = rotationValue[1].threshold[i] > 0.0f && rotationValue[1].state;
            bool bool2 = rotationValue[2].threshold[i] > 0.0f && rotationValue[2].state;
            
            if (bool0 || bool1 || bool2){
                juce::Colour myColour = juce::Colour(255, 255, 255);
                g.setColour(myColour);
            }
            g.strokePath(graphicPath, juce::PathStrokeType(2));
        }
    }
    
    // outer rings
    
}

void SpinnerGraphics::setParams(int index, float phaseValue, int divisionValue, bool stateValue, float opacityValue)
{
    rotationValue[index].phase = phaseValue;
    rotationValue[index].division = divisionValue;
    rotationValue[index].state = stateValue;
    rotationValue[index].opacity = opacityValue;

    repaint();
}

