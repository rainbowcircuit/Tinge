/*
  ==============================================================================

    MIDIProcessor.h
    Created: 13 May 2025 8:35:44pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "DSP.h"

class MIDIProcessor
{
public:
    void prepareToPlay(double sampleRate);
    void holdPitches(juce::MidiBuffer &m);
    void notePlayback(juce::MidiBuffer& midiBuffer, int samplePosition, bool gate);
    void setDirection(bool direction);
    
    void noteOn(juce::MidiBuffer& midiBuffer, int samplePosition, int noteNumber, int noteVelocity);
    void noteOff(juce::MidiBuffer& midiBuffer, int samplePosition, int noteNumber);

    void setSpinnerValues(int index, bool state, float phase, float division, float opacity)
    {
        rotationValue[index].state = state;
        rotationValue[index].phase = phase;
        rotationValue[index].division = division;
        rotationValue[index].opacity = opacity;
    }
    
    void spinnerInteraction(juce::MidiBuffer& midiBuffer, int samplePosition)
    {
        if(numNoteOn == 0)
            return;        
        
        for (int j = 0; j < numNoteOn; ++j)
        {
            auto& note = noteValue[j];
            float triggerPos = (1.0f / numNoteOn) * j;

                bool threshold1 = false;
                bool threshold2 = false;
                bool threshold3 = false;

            for (int k = 0; k < rotationValue[0].division; ++k)
                threshold1 = rotationValue[0].threshold(k, triggerPos) && rotationValue[0].state;

                for (int l = 0; l < rotationValue[1].division; ++l)
                threshold2 = rotationValue[1].threshold(l, triggerPos) && rotationValue[1].state;
            
            for (int m = 0; m < rotationValue[2].division; ++m)
                threshold3 = rotationValue[2].threshold(m, triggerPos) && rotationValue[2].state;
            
                bool threshold = threshold1 || threshold2 || threshold3;
            
                if (threshold && !note.isOn && !note.isAvailable) {
             //       DBG(note.noteNumber);
                    noteOn(midiBuffer, samplePosition, note.noteNumber, 1.0f);
                    note.isOn = true;
                    
                } else if (!threshold && note.isOn && !note.isAvailable) {
                    noteOff(midiBuffer, samplePosition, note.noteNumber);
                    note.isOn = false;
                }
            }
    }

    
private:
    double sampleRate;
    
    struct NoteValue
    {
        int noteNumber;
        int noteVelocity;
        bool isOn = false;
        bool isAvailable = true;
    };
    
    struct RotationValue
    {
        bool state;
        float phase;
        float division = 2;
        float opacity = 2;
        float size = 1.0f / division;
        
        float wrapPhase(float x) noexcept
        {
            return x - floorf(x);
        }

        bool isWithinRange(float x, float a, float b) noexcept
        {
            if (a < b) return x >= a && x < b;
            return x <= a && x > b;
        }
        
        bool threshold(int i, float position)
        {
            float start = wrapPhase(phase + i * size);
            float end = wrapPhase(phase + (i + 1) * size);

            return isWithinRange(position, start, end);
        }
    };
    
    std::array<RotationValue, 3> rotationValue;
    
    std::array<NoteValue, 16> noteValue;
    juce::SortedSet<int> activeNoteOn;
    int numNoteOn;
};



class Spinner
{
public:
    void setSampleRate(double sampleRate);
    void reset();
    
    void setRate(int rateBPM, float rateFree, bool isSynced);
    void tempo(juce::AudioPlayHead* playhead);

    void setDivision(int division);
    void nudge(float nudgeStrength, int nudgeForward, int nudgeBackward);
    void accumulate();
    float getPhase();
    bool getDirection();
    
    
private:
    LowPassGate forwardLPG, backwardLPG;
    bool isForward = true;
    double sampleRate;
    float phase = 0.0f, previousPhase = 0.0f;
    float bpm = 120.0f;
    int rateBPM;
    float rateFree;
    bool isSynced;
    
    std::array <double, 16> subdivisionMultiplier =
    {
        0.125,
        0.5 / 3.0,
        0.1875,
        0.25,
        1.0 / 3.0,
        0.375,
        0.5,
        2.0 / 3.0,
        0.75,
        1.0,
        4.0 / 3.0,
        1.5,
        2.0,
        8.0 / 3.0,
        3.0,
        4.0
    };
    
};



