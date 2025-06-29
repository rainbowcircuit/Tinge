
#include "MIDIProcessor.h"

void MIDIProcessor::prepareToPlay(double sampleRate)
{
    this->sampleRate = sampleRate;
    
        for (int i = 0; i < noteValue.size(); i++)
    {
        noteValue[i].noteNumber = -1;
        noteValue[i].isAvailable = true;

    }
}

void MIDIProcessor::holdPitches(juce::MidiBuffer &buffer)
{
    juce::MidiBuffer filteredBuffer;
    for (const auto metadata : buffer)
    {
        const auto message = metadata.getMessage();

        if (message.isNoteOn())
        {
            for (auto& note : noteValue)
            {
                if (note.isAvailable)
                {
                    note.noteNumber = message.getNoteNumber();
                    note.noteVelocity = message.getVelocity();
                    note.isAvailable = false;
                    break;
                }
            }
        }
        else if (message.isNoteOff())
        {
            for (auto& note : noteValue)
            {
                if (!note.isAvailable && note.noteNumber == message.getNoteNumber())
                {
                    note.noteNumber = -1;
                    note.noteVelocity = message.getVelocity();
                    note.isAvailable = true;
                    break;
                }
            }
        }
    }
    buffer.swapWith(filteredBuffer);
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
