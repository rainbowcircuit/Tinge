#pragma once
#include <JuceHeader.h>

class LowPassGate
{
public:
    void setSampleRate(double sampleRate)
    {
        this->sampleRate = sampleRate;
    }

    void setEnvelopeSlew(float riseInMilliseconds, float fallInMilliseconds)
    {
        envelopeRise = (riseInMilliseconds/1000.0f) * sampleRate;
        envelopeFall = (fallInMilliseconds/1000.0f) * sampleRate;
    }

    void triggerEnvelope(bool gateInput)
    {
        gate = gateInput ? 1.0f : 0.0f;
        lastGate = gate;
    }
    
    void triggerEnvelope(float gateInput)
    {
        gate = gateInput;
        lastGate = gate;
    }
    
    float generateEnvelope()
    {
        bool gateThreshold = gate >= 0.5f;
        
        float segmentSelect = gateThreshold ? envelopeRise : envelopeFall;
        segmentSelect = std::max(segmentSelect, 1.0f);
        
        float segmentTimeT60 = processT60(segmentSelect);
        envelope = (1.0f - segmentTimeT60) * gate + segmentTimeT60 * unitDelay;
        if (std::abs(envelope) < 1e-15f) // denormal clearing
            envelope = 0.0f;
        
        unitDelay = envelope;
        return envelope;
    }
    
    bool isActive()
    {
        return envelope > 0.0001f;
    }

private:
    float processT60(float input)
    {
        float t60 = std::exp(safediv(-6.9077552789821f, input));
        return t60;
    }

    float safediv(float numerator, float denominator)
    {
        return denominator != 0.0f ? numerator / denominator : numerator / epsilon;
    }
    
    int count = 0;
    double sampleRate = 0.0;
    float unitDelay = 0.0f;
    static constexpr float epsilon = 1e-6f; // safety for dividing by 0
    float envelopeRise = 0.0f, envelopeFall = 0.0f;
    float gate = 0.0f, lastGate = 0.0f, envelope = 0.0f;
};

