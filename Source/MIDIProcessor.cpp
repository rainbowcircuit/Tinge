
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
    
    if (!hold)
    {
        for (auto& note : noteValue)
        {
            if (note.isHeld)
            {
                auto noteOffMessage = juce::MidiMessage::noteOff(1, note.noteNumber);
                filteredBuffer.addEvent(noteOffMessage, 0);
                
                note.noteNumber = -1;
                note.noteVelocity = 0;
                note.isAvailable = true;
                note.isHeld = false;
            }
        }
    }
    
    
    
    for (const auto metadata : buffer)
    {
        const auto message = metadata.getMessage();
        bool addToBuffer = true;
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
                    if (hold)
                    {
                        note.isHeld = true;
                        addToBuffer = false;
                        
                    } else {
                            note.noteNumber = -1;
                            note.noteVelocity = message.getVelocity();
                            note.isAvailable = true;
                            note.isHeld = false;
                    }
                    break;
                }
            }
        }
        
        if(addToBuffer)
        {
            filteredBuffer.addEvent(message, metadata.samplePosition);
        }
    }
    buffer.swapWith(filteredBuffer);
}

void MIDIProcessor::setHold(juce::MidiBuffer &buffer, bool guiHold)
{
    for (const auto metadata : buffer)
    {
        const auto message = metadata.getMessage();
        
        if(message.isController() && message.getControllerNumber() == 64)
        {
            midiHold = message.getControllerValue() >= 64;
        }
    }
    hold = midiHold || guiHold;
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


void MIDIProcessor::noteOn(juce::MidiBuffer& midiBuffer, int channel, int samplePosition, int noteNumber, int noteVelocity)
{
    juce::MidiMessage noteOn = juce::MidiMessage::noteOn(channel, noteNumber, uint8_t(noteVelocity));
    midiBuffer.addEvent(noteOn, samplePosition);
}

void MIDIProcessor::noteOff(juce::MidiBuffer& midiBuffer, int channel, int samplePosition, int noteNumber)
{
    noteNumber = juce::jlimit(0, 127, noteNumber);
    juce::MidiMessage noteOff = juce::MidiMessage::noteOff(channel, noteNumber);
    midiBuffer.addEvent(noteOff, samplePosition);
}


void Spinner::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    this->sampleRate = sampleRate;
    this->samplesPerBlock = samplesPerBlock;
    forwardLPG.setSampleRate(sampleRate);
    backwardLPG.setSampleRate(sampleRate);
    brakeLPG.setSampleRate(sampleRate);

}

void Spinner::setRate(int rateSync, float rateFree, bool rateMode, float phase, float curve)
{
    this->rateSync = rateSync;
    this->rateFree = rateFree;
    this->rateMode = rateMode;
    this->phaseOffset = phase;
    this->curve = curve/100.0f;
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

void Spinner::playhead(juce::AudioPlayHead* playhead)
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
    
    // playhead and reset
    bool isPlaying = pos.getIsPlaying();
    if (!isPlaying && prevIsPlaying) {
        reset();
    }
    prevIsPlaying = isPlaying;
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
    
    if (!holdAccum){
        if (!rateMode) // sync
        {            
            float bpmInHz = ((bpm/60.0f) * rateSyncOptions[rateSync]) + nudgeValue;
            phase += (bpmInHz/sampleRate) * brakeValue;
            
        } else {
            float rateInHz = rateFree + nudgeValue;
            phase += (rateInHz/sampleRate) * brakeValue;
            
        }
    }
    
    if (phase >= 1.0f) phase = 0.0f;
    previousPhase = phase;
}

float Spinner::getPhase()
{
    float offset = fmodf(phase + phaseOffset/100.0f, 1.0f);
    float phaseOutput = getWrappedPhase(offset);
    phaseOutput = getCurvedPhase(phaseOutput);
    
    return phaseOutput;
}

float Spinner::getCurvedPhase(float phase)
{
    if (curve == 1.0f) return phase;
    
    if (curve > 1.0f)
    {
        return std::pow(phase, curve);
    } else {
        return 1.0f - pow(1.0f - phase, curve);
    }
}

float Spinner::getWrappedPhase(float phase)
{
    if (phase < 0.0f) // offset nevative phase values
    {
        phase += 1.0f;
    }
    return phase;
}

bool Spinner::getDirection()
{
    return phase >= previousPhase;
}
