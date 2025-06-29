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
#include "InteractionLogic.h"

class MIDIProcessor : public Interaction
{
public:

    
    
    void prepareToPlay(double sampleRate);
    void holdPitches(juce::MidiBuffer &m);
    int nextAvailableIndex = 0;
    
    
    std::array<float, 16> getheldPitches()
    {
        std::array<float, 16> heldPitches = {};
        for (int i = 0; i < heldPitches.size(); i++)
        {
            heldPitches[i] = noteValue[i].noteNumber;
        }
        return heldPitches;
    }
    
    int getNumHeldNotes()
    {
        int noteCount = 0;
        for(int i = 0; i < 16; i++)
        {
            if (!noteValue[i].isAvailable)
            {
                noteCount++;
            }
        }
        return noteCount;
    }
    
    void noteOn(juce::MidiBuffer& midiBuffer, int samplePosition, int noteNumber, int noteVelocity);
    void noteOff(juce::MidiBuffer& midiBuffer, int samplePosition, int noteNumber);

    void processInteraction()
    {
        setNumThresholds(getheldPitches());
        processThreshold();
        processAngles();
    }
        
    void notePlayback(juce::MidiBuffer& midiBuffer, int samplePosition)
    {
        for (int i = 0; i < numThresholds; i++)
        {
            bool bool0 = rotationValue[0].threshold[i] > 0.0f && rotationValue[0].state;
            bool bool1 = rotationValue[1].threshold[i] > 0.0f && rotationValue[1].state;
            bool bool2 = rotationValue[2].threshold[i] > 0.0f && rotationValue[2].state;
            
            if ((bool0 || bool1 || bool2) && !noteValue[i].isOn)
            {
                noteOn(midiBuffer, samplePosition, noteValue[i].noteNumber, 128);
                noteValue[i].isOn = true;
                
            } else if (!(bool0 || bool1 || bool2) && noteValue[i].isOn){
                noteOff(midiBuffer, samplePosition, noteValue[i].noteNumber);
                noteValue[i].isOn = false;
            }
        }
    }
    
    void setSpinnerValues(int index, bool state, float phase, float division, float opacity)
    {
        rotationValue[index].state = state;
        rotationValue[index].phase = phase;
        rotationValue[index].division = division;
    }
    
    double sampleRate;
  //  int numThresholds;
    struct NoteValue
    {
        int noteNumber;
        int noteVelocity;
        bool isOn = false;
        bool isAvailable = true;
    };

    
    std::array<NoteValue, 16> noteValue;
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



