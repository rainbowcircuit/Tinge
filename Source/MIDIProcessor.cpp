
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

std::array<float, 16> MIDIProcessor::getheldPitches()
{
    std::array<float, 16> heldPitches = {};
    for (int i = 0; i < heldPitches.size(); i++)
    {
        heldPitches[i] = noteValue[i].noteNumber;
    }
    return heldPitches;
}

int MIDIProcessor::getNumHeldNotes()
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
    brakeLPG.setSampleRate(sampleRate);

}

void Spinner::setRate(int rateBPM, float rateFree, bool rateMode, float phaseOffset)
{
    this->rateBPM = rateBPM;
    this->rateFree = rateFree;
    this->rateMode = rateMode;
    this->phaseOffset = phaseOffset;
}

void Spinner::reset()
{
    phase = 0.0f;
}

void Spinner::tempo(juce::AudioPlayHead* playhead)
{
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

void Spinner::nudge(float nudgeStrength, int nudgeForward, int nudgeBackward, int brake)
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
    brakeLPG.setEnvelopeSlew(sampleRate/50.0f, sampleRate/50.0f);

    forwardLPG.triggerEnvelope((bool)nudgeForward);
    backwardLPG.triggerEnvelope((bool)nudgeBackward);
    brakeLPG.triggerEnvelope((bool)brake);
    
}

void Spinner::accumulate()
{
    float nudgeValue = (forwardLPG.generateEnvelope() + (backwardLPG.generateEnvelope() * -1.0f)) * 10.0f;
    float brakeValue = (1.0f - brakeLPG.generateEnvelope());
    
    if (rateMode) // sync
    {
        if (rateBPM < 0 || rateBPM >= subdivisionMultiplier.size())
            return;
        
        float multiplier = subdivisionMultiplier[rateBPM];
        if (multiplier == 0.0f)
            return;

        float bpmInHz = ((bpm/60.0f) + nudgeValue) * multiplier;
        phase += (bpmInHz/sampleRate) * brakeValue;

    } else {
        float rateInHz = (rateFree + nudgeValue);
        phase += (rateInHz/sampleRate) * brakeValue;

    }
    
    if (phase > 1.0f) phase -= 1.0f;
    previousPhase = phase;
}

float Spinner::getPhase()
{
    float offset = phaseOffset / 100.0f;
    float wrapped = fmodf(phase + offset, 1.0f);
    if (wrapped < 0.0f)
        wrapped += 1.0f;
    return wrapped;
}

bool Spinner::getDirection()
{
    return phase >= previousPhase;
}
