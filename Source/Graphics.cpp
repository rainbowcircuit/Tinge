#include "Graphics.h"

SpinnerGraphics::SpinnerGraphics()
{
    thresholdSlew.setSampleRate(60);
    thresholdSlew.setEnvelopeSlew(800, 800);
    startTimerHz(60);
}

void SpinnerGraphics::paint(juce::Graphics& g) 
{
    setWheelPosition();
    
    // summed wheel
    auto sumA = createWheelPath(g,
                                sumBounds,
                                rotationValue[0].phase,
                                rotationValue[0].phase + 0.5f);

    auto sumB = createWheelPath(g,
                                sumBounds,
                                rotationValue[1].phase,
                                rotationValue[1].phase + 0.5f);
    
    auto sumC = createWheelPath(g,
                                sumBounds,
                                rotationValue[2].phase,
                                rotationValue[2].phase + 0.5f);

    auto sumABAngles = clipValues(rotationValue[0].phase,
                                  fmodf(rotationValue[0].phase + 0.5f, 1.0f),
                                  rotationValue[1].phase,
                                  fmodf(rotationValue[1].phase + 0.5f, 1.0f));
    
    auto sumBCAngles = clipValues(rotationValue[1].phase,
                                  fmodf(rotationValue[1].phase + 0.5f, 1.0f),
                                  rotationValue[2].phase,
                                  fmodf(rotationValue[2].phase + 0.5f, 1.0f));

    auto sumACAngles = clipValues(rotationValue[0].phase,
                                  fmodf(rotationValue[0].phase + 0.5f, 1.0f),
                                  rotationValue[2].phase,
                                  fmodf(rotationValue[2].phase + 0.5f, 1.0f));

    
    auto sumABCAngles = clipValues(rotationValue[0].phase,
                                   fmodf(rotationValue[0].phase + 0.5f, 1.0f),
                                   rotationValue[1].phase,
                                   fmodf(rotationValue[1].phase + 0.5f, 1.0f),
                                   rotationValue[2].phase,
                                   fmodf(rotationValue[2].phase + 0.5f, 1.0f));

    auto sumAB = createWheelPath(g,
                                 sumBounds,
                                 sumABAngles[0],
                                 sumABAngles[1]);
    
    auto sumBC = createWheelPath(g,
                                 sumBounds,
                                 sumBCAngles[0],
                                 sumBCAngles[1]);
    
    auto sumAC = createWheelPath(g,
                                 sumBounds,
                                 sumACAngles[0],
                                 sumACAngles[1]);

    auto sumABC = createWheelPath(g,
                                 sumBounds,
                                 sumABCAngles[0],
                                 sumABCAngles[1]);


    drawWithoutOverlap(g, sumA, colorA, false);
    drawWithoutOverlap(g, sumB, colorB, false);
    drawWithoutOverlap(g, sumC, colorC, false);
    drawDoubleOverlap(g, sumA, sumB, colorAB, false);
    drawDoubleOverlap(g, sumB, sumC, colorBC, false);
    drawDoubleOverlap(g, sumA, sumC, colorAC, false);
    drawTripleOverlap(g, sumA, sumB, sumC, colorABC, false);
    
    
    drawThreshold(g);
    
    
    // single wheel
    auto C = generateWheelPath(g, 2);
    auto B = generateWheelPath(g, 1);
    auto A = generateWheelPath(g, 0);
    
    
    drawWithoutOverlap(g, A, p.colorA, false);
    drawWithoutOverlap(g, B, p.colorB, false);
    drawWithoutOverlap(g, C, p.colorC, false);
    
    drawDoubleOverlap(g, A, B, p.colorAB, false);
    drawDoubleOverlap(g, B, C, p.colorBC, false);
    drawDoubleOverlap(g, A, C, p.colorAC, false);
    
    drawTripleOverlap(g, A, B, C, p.colorABC, false);
}

void SpinnerGraphics::resized()
{
    setWheelPosition();
}

void SpinnerGraphics::setWheelPosition()
{
    auto bounds = getLocalBounds().toFloat();
    float wheelWidth = bounds.getWidth() * 0.75f;
    float wheelMargin = bounds.getWidth() * 0.125f;
    float height = bounds.getHeight();
    float x = bounds.getX();
    float y = bounds.getY();
    
    float yOffset = animationValue * height * 2.0f;
    
    rotationValue[0].bounds.setBounds(x + wheelMargin,
                                      y - yOffset,
                                      wheelWidth,
                                      wheelWidth);
    
    rotationValue[1].bounds.setBounds(x + wheelMargin,
                                      y + height * 0.125f - yOffset,
                                      wheelWidth,
                                      wheelWidth);
    
    rotationValue[2].bounds.setBounds(x + wheelMargin,
                                      y + height * 0.25f - yOffset,
                                      wheelWidth,
                                      wheelWidth);

    float sumYScale = animationValue * height * 0.325f;
    float sumYOffset = (1.0f - animationValue) * height * 0.55f;
    
    sumBounds.setBounds(x + wheelMargin, y - sumYScale + sumYOffset, wheelWidth, wheelWidth);
}

juce::Path SpinnerGraphics::generateWheelPath(juce::Graphics& g, int index)
{
    float x = rotationValue[index].bounds.getX();
    float y = rotationValue[index].bounds.getY();
    float width = rotationValue[index].bounds.getWidth();
    float height = rotationValue[index].bounds.getHeight();
    
    float twopi = juce::MathConstants<float>::twoPi;
    float radius = width/2;
    
    juce::Path wheelPath;

    float startAngle = rotationValue[index].phase * twopi;
    float endAngle = (rotationValue[index].phase + 0.5f) * twopi;

    if (endAngle <= startAngle)
        endAngle += twopi;
        
        wheelPath.startNewSubPath(x + width/2, y + height/isometricSkew);
        wheelPath.addCentredArc(x + width/2, y + height/isometricSkew,
                                radius,
                                radius/isometricSkew,
                                0.0f,
                                startAngle,
                                endAngle);
        wheelPath.closeSubPath();
        wheelPath = wheelPath.createPathWithRoundedCorners(1);

    return wheelPath;
}

juce::Path SpinnerGraphics::generateSegmentedWheelPath(juce::Graphics& g, int startIndex, int endIndex)
{
    float twopi = juce::MathConstants<float>::twoPi;
    float x = rotationValue[startIndex].sumBounds.getX();
    float y = rotationValue[startIndex].sumBounds.getY();
    float width = rotationValue[startIndex].sumBounds.getWidth();
    float height = rotationValue[startIndex].sumBounds.getHeight();
    float radius = width/2;
    
    juce::Path wheelPath;

    float startAngle  = rotationValue[startIndex].phase;
    float clipStartAngle  = rotationValue[endIndex].phase;
    float endAngle  = rotationValue[startIndex].phase + 0.5f;
    float clipEndAngle  = rotationValue[endIndex].phase + 0.5f;

    startAngle = std::max(startAngle, clipStartAngle);
    endAngle = std::min(endAngle, clipEndAngle);
    
    startAngle *= twopi;
    endAngle *= twopi;

        wheelPath.startNewSubPath(x + width/2, y + height/isometricSkew); // center
        wheelPath.addCentredArc(x + width/2, y + height/isometricSkew,
                                radius, radius/isometricSkew,
                                0.0f, startAngle, endAngle);
        wheelPath.closeSubPath();
        wheelPath = wheelPath.createPathWithRoundedCorners(1);

    return wheelPath;
}




void SpinnerGraphics::drawThreshold(juce::Graphics& g)
{
    float x = sumBounds.getX();
    float y = sumBounds.getY();
    float width = sumBounds.getWidth();
    float height = sumBounds.getHeight();
    
    float pi = juce::MathConstants<float>::pi;
    float twopi = pi * 2.0f;
    
    float innerRadius = width * 0.525f;
    float outerRadius = width * 0.575f;
    
    for (int i = 0; i < numThresholds; i++)
    {
        float weightA = ((float)rotationValue[0].threshold[i] * rotationValue[0].opacity)/3.0f;
        float weightB = ((float)rotationValue[1].threshold[i] * rotationValue[1].opacity)/3.0f;
        float weightC = ((float)rotationValue[2].threshold[i] * rotationValue[2].opacity)/3.0f;
        float thresholdWeight = (weightA + weightB + weightC);
        
        
        thresholdSlew.triggerEnvelope(thresholdWeight);
        float isometric = (1.0f - animationValue);

        float thresholdHeight = thresholdSlew.generateEnvelope() * height * 0.25f;
        thresholdHeight *= isometric;
        
        if (numThresholds > 0 && numThresholds <= 16)
        {
            float angleOffset = fmodf(thresholdAngles[i] - pi - 0.1, 1.0f);
            float cos = std::cos(angleOffset * twopi);
            float sin = std::sin(angleOffset * twopi);

            juce::Point thresholdStart = { (x + width/2) + cos * innerRadius,
                (y + height/isometricSkew) + sin * innerRadius/isometricSkew };
            
            juce::Point thresholdEnd = { (x + width/2) + cos * outerRadius,
                (y + height/isometricSkew) + sin * outerRadius/isometricSkew };

            juce::Path graphicPath;
            graphicPath.startNewSubPath(thresholdStart.x, thresholdStart.y);
            graphicPath.lineTo(thresholdEnd.x, thresholdEnd.y);
            graphicPath.lineTo(thresholdEnd.x, thresholdEnd.y - thresholdHeight);
            graphicPath.lineTo(thresholdStart.x, thresholdStart.y - thresholdHeight);
            graphicPath.closeSubPath();
            graphicPath = graphicPath.createPathWithRoundedCorners(3.0f);
            
            bool triggerCondition = getTriggerCondition(i, overlapIndex);
            
            auto thresholdColor = triggerCondition ? Colors::graphicWhite : Colors::graphicBlack;

            g.setColour(thresholdColor);
            g.fillPath(graphicPath);
            auto strokeType = juce::PathStrokeType(2.0f, juce::PathStrokeType::JointStyle::curved);
            g.strokePath(graphicPath, strokeType);
        }
    }
}

void SpinnerGraphics::setThresholdSlew(float slewTime)
{
    thresholdSlew.setEnvelopeSlew(slewTime, slewTime);
}

void SpinnerGraphics::setOverlapIndex(int overlapIndex)
{
    this->overlapIndex = overlapIndex;
    setOverlapColours();
}

void SpinnerGraphics::setOverlapColours()
{
    if (overlapIndex == 0 || overlapIndex == 3 || overlapIndex == 4 || overlapIndex == 6){
        colorA = p.colorA;
        colorB = p.colorB;
        colorC = p.colorC;
    } else {
        colorA = Colors::backgroundFill;
        colorB = Colors::backgroundFill;
        colorC = Colors::backgroundFill;
    }
    
    if (overlapIndex == 1 || overlapIndex == 3 || overlapIndex == 5 || overlapIndex == 6){
        colorAB = p.colorAB;
        colorBC = p.colorBC;
        colorAC = p.colorAC;
    } else {
        colorAB = Colors::backgroundFill;
        colorBC = Colors::backgroundFill;
        colorAC = Colors::backgroundFill;
    }
    
    if (overlapIndex == 2 || overlapIndex == 4 || overlapIndex == 5 || overlapIndex == 6){
        colorABC = p.colorABC;
    } else {
        colorABC = Colors::backgroundFill;
    }
}
    
void SpinnerGraphics::setParams(int index, float phase, float opacity1, float opacity2, float opacity3)
{
    rotationValue[index].phase = phase;
    p.setColors(opacity1/100.0f,
                opacity2/100.0f,
                opacity3/100.0f);
    repaint();
}

void SpinnerGraphics::setAnimation(float value)
{
    animationValue = value;
    isometricSkew = (1.25f - (animationValue * 1.25f)) + 1.0f;
}

void SpinnerGraphics::setAnimationValue(float animationValue)
{
    this->animationValue = animationValue;
}

void SpinnerGraphics::timerCallback()
{
    repaint();
}
