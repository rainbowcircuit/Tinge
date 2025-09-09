
#pragma once
#include <JuceHeader.h>

class Interaction 
{
public:
    Interaction(){
        // spit some random value here
        juce::Random rand;
        randomSeed = rand.nextInt();
    }
    
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
    
    enum thresholdMode { EquiDistant = 0, Fill, Harmonic, Clusters, Fibonacci};
    
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
                for (int i = 0; i < numThresholds; i++){
                    float cluster = (randomSeed % 1000) / 1000.0f;
                    float offset = ((randomSeed % 1000) / 1000.0f - 0.5f) * 0.05f;
                    
                    thresholdAngles[i] = std::fmod(cluster + offset + thresholdPhase, 1.0f);
                }
                break;
            }
                
            case thresholdMode::Fibonacci:
            {
                for (int i = 0; i < numThresholds; i++){
                    float fibonacciValues = fibonacci[i];
                    
                    thresholdAngles[i] = std::fmod(fibonacciValues + thresholdPhase, 1.0f);
                }
                break;
            }

            
            default: // equidistant
            {
                float thresholdIncr = 1.0f/numThresholds;
                for (int i = 0; i < numThresholds; i++){
                    float angles = thresholdIncr * i;
                    thresholdAngles[i] = std::fmodf(angles + thresholdPhase, 1.0f);
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
    
    int randomSeed;
    std::array<double, 16> fibonacci = {
        0.000000,  // 1 -> 0
        0.000000,  // 1 -> 0
        0.001015,  // 2 -> 0.001015
        0.002030,  // 3 -> 0.002030
        0.004061,  // 5 -> 0.004061
        0.007099,  // 8 -> 0.007099
        0.012173,  // 13 -> 0.012173
        0.020284,  // 21 -> 0.020284
        0.033468,  // 34 -> 0.033468
        0.054777,  // 55 -> 0.054777
        0.089249,  // 89 -> 0.089249
        0.145030,  // 144 -> 0.145030
        0.235193,  // 233 -> 0.235193
        0.381340,  // 377 -> 0.381340
        0.617749,  // 610 -> 0.617749
        0.999999   // 987 -> 1
    };

    
};
