
#pragma once
#include <JuceHeader.h>

class Interaction // this class is meant for both DSP and Graphics maybe
{
public:

    std::array<float, 16> getRotationAngles(float phase, int division)
    {
        std::array<float, 16> outputAngles = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        if (division < 0) return outputAngles;
        
        int numSegments = division * 2;
        float segmentSize = 1.0f/numSegments;
        
        outputAngles[0] = fmodf(phase, 1.0f);
        for (int i = 1; i < numSegments; i++)
        {
            float angle = fmodf((segmentSize * i + phase), 1.0f);
            outputAngles[i] = angle;
        }

        return outputAngles;
    }
    
    std::array<float, 16> getInteraction(std::array<float, 16> angles, std::array<float, 16> &thresholdAngles, int division)
    {
        std::array<float, 16> thresholdBools;
        for (int i = 0; i < thresholdBools.size(); i++)
        {
            thresholdBools[i] = 0.0f;
        }

        for (int i = 0; i < division * 2 - 1; i++) {
            if (i % 2 == 0) {
                for (int j = 0; j < 6; j++) {
                    if (isOverThreshold(thresholdAngles[j], angles[i], angles[i + 1]))
                        thresholdBools[j] = 1.0f;
                }
            }
        }
        return thresholdBools;
    }
    
    bool isOverThreshold(float value, float start, float end) {
        if (start < end) {
            return value > start && value <= end;
        } else {
            return value > start || value <= end;
        }
    }
    
    
    void processThreshold()
    {
        if (numThresholds == 0)
            return;
        
        // find the default angles of thresholds
        for (int i = 0; i < 16; i++){
            float thresholdIncr = 1.0f/numThresholds;
            thresholdAngles[i] = thresholdIncr * i;
        }
        
        // find the interaction of thresholds
        for (int index = 0; index < 3; index++)
        {
            rotationValue[index].threshold = getInteraction(rotationValue[index].angles,
                                                            thresholdAngles,
                                                            rotationValue[index].division);
        }
    }

    void processAngles()
    {
        for (int index = 0; index < 3; index++)
        {
            rotationValue[index].angles = getRotationAngles(rotationValue[index].phase, rotationValue[index].division);
        }
    }
    

    void setNumThresholds(std::array<float, 16> heldNotes)
    {
        int thresCounter = 0;
        for (int i = 0; i < 16; i++){
            thresholdValues[i].noteNumber = heldNotes[i];
            
            if (thresholdValues[i].noteNumber != -1)
            {
                thresCounter++;
            }
        }
        numThresholds = thresCounter;
    }

    
    

    // eventually three of these per rotation
    int numThresholds;
    
    std::array<float, 32> angles;
    
    struct RotationValue
    {
        bool state = true;
        float phase = 0.0f;
        float division;
        std::array<float, 16> angles;
        std::array<float, 16> threshold;
        
        float opacity;
        juce::Colour color = {255, 255, 255};
    };
    std::array<RotationValue, 3> rotationValue;

    struct Threshold
    {
        float anglePosition;
        bool threshold;
        int noteNumber;
    };
    
    std::array<Threshold, 16> thresholdValues;
    std::array<float, 16> thresholdAngles;

    
private:
    
};
