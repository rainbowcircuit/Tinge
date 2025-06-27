/*
  ==============================================================================

    MIDIProcessor.cpp
    Created: 13 May 2025 8:35:44pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#include "MIDIProcessor.h"

void MIDIProcessor::prepareToPlay(double sampleRate)
{
    this->sampleRate = sampleRate;
    
    
    // test populating with a Cmaj9
    for (int i = 0; i < noteValue.size(); i++)
    {
        noteValue[i].noteNumber = -1;
        noteValue[i].isAvailable = true;

    }
    numNoteOn = 7;

}
/*
void MIDIProcessor::holdPitches(juce::MidiBuffer &m)
{
    juce::MidiBuffer filteredBuffer;
    for (const juce::MidiMessageMetadata metadata : m)
    {
        auto message = metadata.getMessage();
        
        if(message.isNoteOn())
        {
       //     latchedNote = message.getNoteNumber();
         //   latchedNotes.add(latchedNote);
//            hasLatchedNote = true;
            
        } else if (message.isNoteOff())
        {
     //       latchedNote = message.getNoteNumber();
      //      latchedNotes.removeValue(latchedNote);
            
        } else
        {
            filteredBuffer.addEvent(message, metadata.samplePosition);
            
        }
   //     hasLatchedNote = latchedNotes.isEmpty();
    }
    
    m.swapWith(filteredBuffer);
}
*/

void MIDIProcessor::holdPitches(juce::MidiBuffer &m)
{
    juce::MidiBuffer filteredBuffer;
    for (const juce::MidiMessageMetadata metadata : m)
    {
        auto message = metadata.getMessage();
        
        for (int i = 0; i < noteValue.size(); i++){
            auto& note = noteValue[i];
            if(message.isNoteOn() && note.isAvailable)
            {
                int noteNumber = message.getNoteNumber();
              //  if (juce::isPositiveAndBelow(noteNumber, 128))
             //   {
                    noteValue[i].noteNumber = noteNumber;
                    noteValue[i].isAvailable = false;
            //    }
                
            } else if (message.isNoteOff() && note.noteNumber == message.getNoteNumber()) {

                noteValue[i].noteNumber = -1;
                noteValue[i].isAvailable = true;

            } else
            {
                filteredBuffer.addEvent(message, metadata.samplePosition);
            }
        }
    }
    m.swapWith(filteredBuffer);

    
    /*
    juce::MidiBuffer filteredBuffer;
    for (const juce::MidiMessageMetadata metadata : m)
    {
        auto message = metadata.getMessage();
        numNoteOn = 0;
        for (int i = 0; i < noteValue.size(); i++){
            auto& note = noteValue[i];
            if(message.isNoteOn() && note.isAvailable)
            {
                int noteNumber = message.getNoteNumber();
                if (juce::isPositiveAndBelow(noteNumber, 128))
                {
                    note.noteNumber = noteNumber;
                    note.isAvailable = false;
                }
                
            } else if (message.isNoteOff() && note.noteNumber == message.getNoteNumber())
            {
                note.noteNumber = 0;
                note.isAvailable = true;
            } else
            {
                filteredBuffer.addEvent(message, metadata.samplePosition);
            }
        }
    }
    
    m.swapWith(filteredBuffer);
     */
    for (int i = 0; i < activeNoteOn.size(); i++)
    {
  //      DBG("index: " << i << "notenumber: " << noteValue[i].noteNumber);
    }
}


void MIDIProcessor::notePlayback(juce::MidiBuffer& midiBuffer, int samplePosition, bool gate)
{
    /*
    if(gate && !previousGate)
    {
//        int noteToPlay = latchedNotes[playbackIndex];
        noteOn(midiBuffer, samplePosition, noteToPlay, 120);
        activeNote = noteToPlay;
        
        if(!hasLatchedNote)
        {
            playbackIndex += indexAccum;
            
            if (playbackIndex >= latchedNotes.size()) {
                playbackIndex = 0;
            } else if (playbackIndex < 0) {
                playbackIndex = latchedNotes.size();
            }
            
        } else {
            hasLatchedNote = false;
        }
    }
    
    else if (!gate && previousGate && activeNote >= 0)
    {
        noteOff(midiBuffer, samplePosition, activeNote);
        activeNote = -1;
    }
    
    previousGate = gate;
     */
}

void MIDIProcessor::setDirection(bool direction)
{
  //  this->direction = direction;
 //   indexAccum = direction ? 1 : -1;
}


void MIDIProcessor::noteOn(juce::MidiBuffer& midiBuffer, int samplePosition, int noteNumber, int noteVelocity)
{
    juce::MidiMessage noteOn = juce::MidiMessage::noteOn(1, noteNumber, uint8_t(127));
    midiBuffer.addEvent(noteOn, samplePosition);
}

void MIDIProcessor::noteOff(juce::MidiBuffer& midiBuffer, int samplePosition, int noteNumber)
{
    juce::MidiMessage noteOff = juce::MidiMessage::noteOff(1, noteNumber);
    midiBuffer.addEvent(noteOff, samplePosition);
}


void Spinner::setSampleRate(double sampleRate)
{
    this->sampleRate = sampleRate;
    forwardLPG.setSampleRate(sampleRate);
    backwardLPG.setSampleRate(sampleRate);
}

void Spinner::setRate(int rateBPM, float rateFree, bool isSynced)
{
    this->rateBPM = rateBPM;
    this->rateFree = rateFree;
    this->isSynced = isSynced;
}

void Spinner::reset()
{
    phase = 0.0f;
}

void Spinner::tempo(juce::AudioPlayHead* playhead)
{
    reset();
    if (playhead == nullptr){
        return;
    }
    
    const auto opt = playhead->getPosition();
    if (!opt.hasValue()) {
        return;
    }
    
    const auto& pos = *opt;
    if (pos.getBpm().hasValue()) {
        bpm = *pos.getBpm();
    }
}

void Spinner::setDivision(int division)
{
    
}

void Spinner::nudge(float nudgeStrength, int nudgeForward, int nudgeBackward)
{
    float riseScaled = nudgeStrength * 4.0f;
    float fallScaled = nudgeStrength;
    
    if (fallScaled <= sampleRate/20.0f)
    {
        fallScaled = sampleRate/20.0f;
    } else if (fallScaled >= sampleRate/2.0f)
    {
        fallScaled = sampleRate/2.0f;
    }
    
    forwardLPG.setEnvelopeSlew(riseScaled, fallScaled);
    backwardLPG.setEnvelopeSlew(riseScaled, fallScaled);

    forwardLPG.triggerEnvelope(nudgeForward);
    backwardLPG.triggerEnvelope(nudgeBackward);
}

void Spinner::accumulate()
{
    float nudgeValue = (forwardLPG.generateEnvelope() + (backwardLPG.generateEnvelope() * -1.0f)) * 6.0f;

    if (!isSynced)
    {
        phase += (rateFree + nudgeValue)/sampleRate;
        
    } else {
        float bpmInHz = (bpm/60.0f) + nudgeValue * subdivisionMultiplier[rateBPM];
        phase += bpmInHz;
    }
    if (phase > 1.0f) phase -= 1.0f;

    previousPhase = phase;
}

float Spinner::getPhase()
{
    return phase;
}

bool Spinner::getDirection()
{
    return phase >= previousPhase;
}
