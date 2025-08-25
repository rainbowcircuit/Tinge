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
    
    std::array<float, 16> getheldPitches();
    int getNumHeldNotes();
    
    void noteOn(juce::MidiBuffer& midiBuffer, int channel, int samplePosition, int noteNumber, int noteVelocity);
    void noteOff(juce::MidiBuffer& midiBuffer, int channel, int samplePosition, int noteNumber);

    void processInteraction()
    {
        setNumThresholds(getheldPitches());
        processThreshold();
        processAngles();
    }
        
    void setScaling(int noteScale, float velocityScale, float controllerScale, float slewAmount)
    {
        this->noteScale = noteScale;
        this->velocityScale = velocityScale;
        this->controllerScale = controllerScale;
        this->slewAmount = slewAmount;
    }
        
    void notePlayback(juce::MidiBuffer& midiBuffer)
    {
        if (numThresholds <= 0)
            flushNote(midiBuffer);
        
        for (int i = 0; i < numThresholds; i++)
        {
            const bool triggerCondition = getTriggerCondition(i, overlap);
            
            float weightA = ((float)rotationValue[0].threshold[i] * rotationValue[0].opacity);
            float weightB = ((float)rotationValue[1].threshold[i] * rotationValue[1].opacity);
            float weightC = ((float)rotationValue[2].threshold[i] * rotationValue[2].opacity);
            float thresholdWeight = (weightA + weightB + weightC)/3.0f;
            
            int noteScaled = noteValue[i].noteNumber + (noteScale * thresholdWeight);
            const int currentNote = juce::jlimit(0, 127, noteScaled);
            
            int velocityScaled = noteValue[i].noteVelocity + (127 * thresholdWeight);
            const int currentVelocity = juce::jlimit(0, 127, velocityScaled);
            
            int controllerScaled = (127 * thresholdWeight);
            const int currentController = juce::jlimit(0, 127, controllerScaled);

            int mpeChannel = i + 1;
            if (triggerCondition)
            {
                if (!noteValue[i].isOn || noteValue[i].activeNoteNumber != currentNote)
                {
                    // note off message for previously different note on
                    if (noteValue[i].isOn && noteValue[i].activeNoteNumber != -1)
                    {
                        noteOff(midiBuffer, mpeChannel, 0, noteValue[i].activeNoteNumber);
                    }
                    
                    noteOn(midiBuffer, mpeChannel, 0, currentNote, currentVelocity);
                    noteValue[i].isOn = true;
                    noteValue[i].activeNoteNumber = currentNote;
                    
                }
                
                midiBuffer.addEvent(juce::MidiMessage::controllerEvent(mpeChannel, 74, currentController), 0);
                midiBuffer.addEvent(juce::MidiMessage::aftertouchChange(mpeChannel, currentNote, currentController), 0);

            }
            else
            {
                if (noteValue[i].isOn && noteValue[i].activeNoteNumber != -1)
                {
                    noteOff(midiBuffer, mpeChannel, 0, noteValue[i].activeNoteNumber);
                    noteValue[i].isOn = false;
                    noteValue[i].activeNoteNumber = -1;
                }
            }
        }
    }
    
    void flushNote(juce::MidiBuffer& midiBuffer)
    {
        for (int i = 0; i < 16; i++) {
            int noteNumber = noteValue[i].activeNoteNumber;
            int channel = i + 1;
            
            noteOff(midiBuffer, channel, 0, noteNumber);
            noteValue[i].isOn = false;
            noteValue[i].isAvailable = true;
        }
    }
    
    void setSpinnerValues(int index, float phase, float ratio, float opacity)
    {
        rotationValue[index].phase = phase;
        rotationValue[index].ratio = ratio;
        rotationValue[index].opacity = opacity;

    }
    
    void setOverlap(int overlap)
    {
        this->overlap = overlap;
    }
    
private:
    
    double sampleRate;
    int numNoteOn = 0;
    int overlap = 0;
    float noteScale, velocityScale, controllerScale, slewAmount;
    
    struct NoteValue
    {
        int activeNoteNumber;
        int noteNumber;
        int noteVelocity;
        bool isOn = false;
        bool isAvailable = true;
    };

    std::array<NoteValue, 16> noteValue;
};



class Spinner
{
public:
    void setSampleRate(double sampleRate);
    void reset();
    void resetMode(int resetMode, int numHeldNotes, bool manualReset);

    void setRate(int rateBPM, float rateFree, bool isSynced, float phaseOffset, float rateScale);
    void tempo(juce::AudioPlayHead* playhead);

    void nudge(float nudgeStrength, int nudgeForward, int nudgeBackward, float brakeStrength, int brake);

    void accumulate();
    float getPhase();
    bool getDirection();
    
    
private:
    LowPassGate forwardLPG, backwardLPG, brakeLPG;
    double sampleRate;
    float phase = 0.0f, previousPhase = 0.0f, phaseOffset = 0.0f, rateScale = 1.0f;
    float bpm = 120.0f;
    int rateBPM = 0;
    float rateFree = 0.0f;
    bool rateMode = false, holdAccum = false, manualReset = false, previousManualReset = false;
    
    std::array <double, 7> rateScaleMultiplier =
    {
        0.125,
        0.25f,
        0.5f,
        1.0f,
        2.0f,
        4.0f,
        8.0f };

    std::array <double, 39> subdivisionMultiplier =
    {
        -8.0,
        -6.0,
        -5.3333333,
        -4.0,
        -3.0,
        -2.6666667,
        -2.0,
        -1.5,
        -1.3333333,
        -1.0,
        -0.75,
        -0.6666667,
        -0.5,
        -0.375,
        -0.3333333,
        -0.25,
        -0.1666667,
        -0.125,
        -0.0833333,
        0.0f,
        0.0833333,
        0.125,
        0.1666667,
        0.25,
        0.3333333,
        0.375,
        0.5,
        0.6666667,
        0.75,
        1.0,
        1.3333333,
        1.5,
        2.0,
        2.6666667,
        3.0,
        4.0,
        5.3333333,
        6.0,
        8.0 };
};

