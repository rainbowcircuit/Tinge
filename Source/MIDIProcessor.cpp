
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

void Spinner::setRate(int rateBPM, float rateFree, bool rateMode, float phaseOffset, float rateScale)
{
    this->rateBPM = rateBPM;
    this->rateFree = rateFree;
    this->rateMode = rateMode;
    this->phaseOffset = phaseOffset;
    this->rateScale = rateScale;
}

void Spinner::reset()
{
    phase = phaseOffset;
}

void Spinner::resetMode(int resetMode, int numHeldNotes, bool manualReset) // toggle, momentary
{
    if (manualReset && !previousManualReset)
        reset();
    
    previousManualReset = manualReset;
    
    holdAccum = false;
    if (resetMode == 1 && numHeldNotes == 0) { // hold and reset
        reset();
        holdAccum = true;
    } else if (resetMode == 2 && numHeldNotes == 0) { // hold no reset
        holdAccum = true;
    }
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

void Spinner::nudge(float nudgeStrength, int nudgeForward, int nudgeBackward, float brakeStrength, int brake)
{
    float nudgeRiseScaled = nudgeStrength * 400.0f;
    float nudgeFallScaled = nudgeStrength * 100.0f;
    
    if (nudgeFallScaled <= sampleRate/20.0f) { nudgeFallScaled = sampleRate/20.0f; }
    else if (nudgeFallScaled >= sampleRate/2.0f) { nudgeFallScaled = sampleRate/2.0f; }
    
    forwardLPG.setEnvelopeSlew(nudgeRiseScaled, nudgeFallScaled);
    backwardLPG.setEnvelopeSlew(nudgeRiseScaled, nudgeFallScaled);
    
    float brakeTimeScaled = brakeStrength * 100.0f;
    
    brakeLPG.setEnvelopeSlew(brakeTimeScaled, brakeTimeScaled);

    // trigger envelope
    forwardLPG.triggerEnvelope((bool)nudgeForward);
    backwardLPG.triggerEnvelope((bool)nudgeBackward);
    brakeLPG.triggerEnvelope((bool)brake);
}


void Spinner::accumulate()
{
    float nudgeValue = (forwardLPG.generateEnvelope() + (backwardLPG.generateEnvelope() * -1.0f)) * 10.0f;
    float brakeValue = (1.0f - brakeLPG.generateEnvelope());
    float scaleMultiplier = rateScaleMultiplier[rateScale];
    if (!holdAccum){
        if (rateMode) // sync
        {
            float multiplier = subdivisionMultiplier[rateBPM];
            
            float bpmInHz = ((bpm/60.0f) * multiplier * scaleMultiplier) + nudgeValue;
            phase += (bpmInHz/sampleRate) * brakeValue;
            
        } else {
            float rateInHz = (rateFree * scaleMultiplier) + nudgeValue;
            phase += (rateInHz/sampleRate) * brakeValue;
            
        }
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
