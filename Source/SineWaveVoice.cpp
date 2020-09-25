#include "SineWaveVoice.h"

void SineWaveVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound*, int /*currentPitchWheelPosition*/)
{
    currentAngle = 0.0;
    level = velocity * 0.15 * overallGain;
    tailOff = 0.0;

    cyclesPerSecond = MidiMessage::getMidiNoteInHertz(midiNoteNumber + midiNoteOffset);
    double cyclesPerSample = cyclesPerSecond / getSampleRate();

    angleDelta = cyclesPerSample * 2.0 * MathConstants<double>::pi;
}

void SineWaveVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        // start a tail-off by setting this flag. The render callback will pick up on
        // this and do a fade out, calling clearCurrentNote() when it's finished.
        if (tailOff == 0.0) // we only need to begin a tail-off if it's not already doing so - the
            // stopNote method could be called more than once.
            tailOff = 1.0;
    }
    else
    {
        // we're being told to stop playing immediately, so reset everything
        clearCurrentNote();
        angleDelta = 0.0;
    }
}

void SineWaveVoice::pitchWheelMoved(int pitchWheelValue)
{
    double pitchDev = 2.0 * (pitchWheelValue - 8192) / 8192.0;
    double cyclesPerSample = cyclesPerSecond * pow(2.0, pitchDev / 12.0) / getSampleRate();
    angleDelta = cyclesPerSample * 2.0 * MathConstants<double>::pi;
}

void SineWaveVoice::renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (angleDelta != 0.0)
    {
        if (tailOff > 0)
        {
            while (--numSamples >= 0)
            {
                const float currentSample = (float)(sin(currentAngle) * level * tailOff);

                for (int i = outputBuffer.getNumChannels(); --i >= 0;)
                    outputBuffer.addSample(i, startSample, currentSample);

                currentAngle += angleDelta;
                ++startSample;

                tailOff *= 0.99;

                if (tailOff <= 0.005)
                {
                    // tells the synth that this voice has stopped
                    clearCurrentNote();

                    angleDelta = 0.0;
                    break;
                }
            }
        }
        else
        {
            while (--numSamples >= 0)
            {
                const float currentSample = (float)(sin(currentAngle) * level);

                for (int i = outputBuffer.getNumChannels(); --i >= 0;)
                    outputBuffer.addSample(i, startSample, currentSample);

                currentAngle += angleDelta;
                ++startSample;
            }
        }
    }
}
