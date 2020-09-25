#pragma once
#include <JuceHeader.h>

class SineWaveSound : public SynthesiserSound
{
public:
    ~SineWaveSound() override = default;

    bool appliesToNote(int /*midiNoteNumber*/) override { return true; }
    bool appliesToChannel(int /*midiChannel*/) override { return true; }
};

class SineWaveVoice : public SynthesiserVoice
{
public:
    SineWaveVoice(int noteOffset, double gain)
        : midiNoteOffset(noteOffset), overallGain(gain)
        , currentAngle(0), angleDelta(0), level(0), tailOff(0) {}
    ~SineWaveVoice() override = default;

    bool canPlaySound(SynthesiserSound* sound) override
    {
        return dynamic_cast<SineWaveSound*> (sound) != nullptr;
    }

    void startNote(int midiNoteNumber, float velocity,
                    SynthesiserSound* /*sound*/, int /*currentPitchWheelPosition*/) override;
    void stopNote(float /*velocity*/, bool allowTailOff) override;

    void pitchWheelMoved(int pitchWheelValue) override;
    void controllerMoved(int /*controllerNumber*/, int /*newValue*/) override {}

    void renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

private:
    int midiNoteOffset;
    double overallGain;

    double cyclesPerSecond; // base pitch, before bending
    double currentAngle, angleDelta, level, tailOff;
};
