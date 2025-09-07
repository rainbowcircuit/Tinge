
#pragma once
#include <JuceHeader.h>

class Interaction 
{
public:

    std::array<bool, 16> getInteraction(float startPhase, float shape, std::array<float, 16> &thresholdAngles)
    {
        std::array<bool, 16> thresholdBool;
        for (int i = 0; i < thresholdBool.size(); i++){
            thresholdBool[i] = false;
        }

        for (int j = 0; j < numThresholds; j++) {
            bool overThreshold = isOverThreshold(thresholdAngles[j],
                                                 startPhase,
                                                 fmodf(startPhase + shape, 1.0f));
            if (overThreshold) { thresholdBool[j] = true; }
        }
        
        return thresholdBool;
    }
    
    bool isOverThreshold(float value, float start, float end) {
        if (start < end) {
            return value > start && value <= end;
        } else {
            return value > start || value <= end;
        }
    }
    
    enum thresholdMode { EquiDistant = 0, Fill, Harmonic, Clusters, Sequential, Fibonacci};
    
    void processThreshold(thresholdMode mode, float thresholdPhase, int maxThreshold)
    {
        thresholdPhase = thresholdPhase/100.0f;
        
        if (numThresholds == 0)
            return;
        
        switch(mode){
            case thresholdMode::EquiDistant:
            {
                float thresholdIncr = 1.0f/numThresholds;
                for (int i = 0; i < numThresholds; i++){
                    float angles = thresholdIncr * i;
                    thresholdAngles[i] = std::fmodf(angles + thresholdPhase, 1.0f);
                }
                break;
            }
                
            case thresholdMode::Fill:
            {
                for (int i = 0; i < numThresholds; i++){
                    float angles = (1.0f/maxThreshold) * i;
                    thresholdAngles[i] = std::fmodf(angles + thresholdPhase, 1.0f);
                }
                break;
            }

            case thresholdMode::Harmonic:
            {
                for (int i = 0; i < numThresholds; i++){
                    float angles = 1.0f - (1.0f / (i + 1));
                    thresholdAngles[i] = std::fmodf(angles + thresholdPhase, 1.0f);
                }
                break;
            }
                                
            case thresholdMode::Clusters:
            {
                
                juce::Random rand;
                int randomSeed = rand.nextInt();
                for (int i = 0; i < numThresholds; i++){
                    float cluster = (randomSeed % 1000) / 1000.0f;
                    float offset = ((randomSeed % 1000) / 1000.0f - 0.5f) * 0.05f;
                    
                    thresholdAngles[i] = std::fmod(cluster + offset + thresholdPhase, 1.0f);
                }
                break;
            }
                
            case thresholdMode::Sequential:
            {
                float pos = 0.0f;
                float step = 1.0f / numThresholds;
                
                for (int i = 0; i < numThresholds; i++){

                    thresholdAngles[i] = pos;
                    pos += step * (1.0f + 0.2f * i);
                    
                    if (pos > 1.0f) pos -= 1.0f;
                }
                break;
            }

        }
        // find the interaction of thresholds
        for (int index = 0; index < 3; index++)
        {
            rotationValue[index].threshold = getInteraction(rotationValue[index].phase, rotationValue[index].shape, thresholdAngles);
            
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

    bool getTriggerCondition(int i, int overlap)
    {
        bool A = rotationValue[0].threshold[i] && rotationValue[0].opacity >= 0.01f;
        bool B = rotationValue[1].threshold[i] && rotationValue[1].opacity >= 0.01f;
        bool C = rotationValue[2].threshold[i] && rotationValue[2].opacity >= 0.01f;
        
        bool onlyA  =  A && !B && !C;
        bool onlyB  = !A &&  B && !C;
        bool onlyC  = !A && !B &&  C;
        bool onlyAB =  A &&  B && !C;
        bool onlyBC = !A &&  B &&  C;
        bool onlyAC =  A && !B &&  C;
        bool allABC = A && B && C;
        
        switch (overlap)
        {
            case 0: return onlyA || onlyB || onlyC;
            case 1: return onlyAB || onlyBC || onlyAC;
            case 2: return allABC;
            case 3: return onlyA || onlyB || onlyC || onlyAB || onlyBC || onlyAC;
            case 4: return onlyA || onlyB || onlyC || allABC;
            case 5: return onlyAB || onlyBC || onlyAC || allABC;
            case 6: return A || B || C; 
            default: return A || B || C;
        }
    }

    // eventually three of these per rotation
    int numThresholds = 0, prevNumThresholds = 0;
    
    std::array<float, 32> angles;
    
    struct RotationValue
    {
        float phase = 0.0f;
        float shape = 0.5f;
        float opacity = 0.8f;
        std::array<bool, 16> threshold;
        
        //******* Graphics Code *******//
        juce::Rectangle<float> sumBounds;
        juce::Rectangle<float> bounds;
        
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
    
    void sigmoid(float slope)
    {
        for (int i = 0; i < numThresholds; i++)
        {
            float y = 1.0f / (1.0f + std::exp(-slope * thresholdAngles[i]));
            thresholdAngles[i] = y;
        }
    }
    
    void linear(float slope)
    {
        for (int i = 0; i < numThresholds; i++){
            float y = thresholdAngles[i] / numThresholds;
            thresholdAngles[i] = y;
        }
    }
    
    
    
};
