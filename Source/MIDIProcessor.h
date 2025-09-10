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
    
    void setHold(juce::MidiBuffer &m, bool hold);
    
    std::array<float, 16> getheldPitches();
    int getNumHeldNotes();
    
    void noteOn(juce::MidiBuffer& midiBuffer, int channel, int samplePosition, int noteNumber, int noteVelocity);
    void noteOff(juce::MidiBuffer& midiBuffer, int channel, int samplePosition, int noteNumber);

    void processInteraction(int thresholdMode, float thresholdPhase, float maxThreshold)
    {
        setNumThresholds(getheldPitches());
        processThreshold(static_cast<enum thresholdMode>(thresholdMode),
                         thresholdPhase,
                         maxThreshold);

    }
        
    void setScaling(int noteScale, float velocityScale, float controllerScale, float slewAmount)
    {
        this->noteScale = noteScale;
        this->velocityScale = velocityScale;
        this->controllerScale = controllerScale;
        this->slewAmount = slewAmount;
    }
        
    float slewValue(float a, float b, float t)
    {
        return a + (b - a) * t;
    }
    
    void notePlayback(juce::MidiBuffer& midiBuffer)
    {
        if (numThresholds <= 0)
            flushNote(midiBuffer); // figure out how to trigger this one
        
        for (int i = 0; i < numThresholds; i++)
        {
            const bool triggerCondition = getTriggerCondition(i, overlap);
            
            float weightA = ((float)rotationValue[0].threshold[i] * rotationValue[0].opacity)/3.0f;
            float weightB = ((float)rotationValue[1].threshold[i] * rotationValue[1].opacity)/3.0f;
            float weightC = ((float)rotationValue[2].threshold[i] * rotationValue[2].opacity)/3.0f;
            float thresholdWeight = weightA + weightB + weightC;
            
            const int currentNote = juce::jlimit(0, 127, noteValue[i].noteNumber);
            
            int velocityScaled = (127 * thresholdWeight);
            const int currentVelocity = juce::jlimit(0, 127, velocityScaled);
            
            int controllerScaled = (127 * thresholdWeight);
            int currentController = juce::jlimit(0, 127, controllerScaled);
            currentController = (int)slewValue(currentController, prevController, slewAmount);
            
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
                
                prevController = currentController;
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
    
    void setSpinnerValues(int index, float phase, float opacity)
    {
        rotationValue[index].phase = phase;
        rotationValue[index].opacity = opacity/100.0f;

    }
    
    void setOverlap(int overlap)
    {
        this->overlap = overlap;
    }
    
    void setSlewAmount(float slewAmount)
    {
        this->slewAmount = slewAmount/100.0f;
    }
    
private:
    
    double sampleRate;
    int numNoteOn = 0;
    int overlap = 0;
    float noteScale, velocityScale, controllerScale, prevController = 0.0f, slewAmount;
    bool hold, midiHold;
    struct NoteValue
    {
        int activeNoteNumber;
        int noteNumber;
        int noteVelocity;
        bool isOn = false;
        bool isAvailable = true;
        bool isHeld = false;
    };

    std::array<NoteValue, 16> noteValue;
};



class Spinner
{
public:
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void reset();
    void resetMode(int resetMode, int numHeldNotes, bool manualReset);

    void setRate(int rateBPM, float rateFree, bool isSynced, float phase, float curve);
    void playhead(juce::AudioPlayHead* playhead);

    void nudge(int nudgeForward, int nudgeBackward, int brake, float jog);

    void accumulate();
    float getPhase();
    float getCurvedPhase(float phase);
    float getWrappedPhase(float phase);
    bool getDirection();
    
    
private:
    LowPassGate forwardLPG, backwardLPG, brakeLPG;
    double sampleRate;
    int samplesPerBlock;
    double phase = 0.0f, previousPhase = 0.0f;
    float phaseOffset = 0.0f, rateScale = 1.0f, curve = 1.0f;
    float bpm = 120.0f;
    
    int rateSync = 0;
    float rateFree = 0.0f;
    bool rateMode = false, holdAccum = false, manualReset = false, previousManualReset = false;
    bool prevIsPlaying = false;
    
    
    static constexpr std::array<double, 39> rateSyncOptions = {
        // Negative rates
        -8.0,       -6.0,        -4.0,      -3.0,
        -2.0,       -1.5,        -1.333,    -1.0,
        -0.8,       -0.75,       -0.666,    -0.5,
        -0.333,     -0.25,       -0.125,    -0.0833,
        -0.0625,    -0.04167,    -0.03125,

         // No motion (middle)
         0.0,
         
         // positive rates
         0.03125,   0.04167,   0.0625,   0.0833,
         0.125,     0.25,      0.333,    0.5,
         0.666,     0.75,      0.8,      1.0,
         1.333,     1.5,       2.0,      3.0,
         4.0,       6.0,       8.0
    };
};
