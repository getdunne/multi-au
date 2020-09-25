#pragma once
#include <JuceHeader.h>

class MultiOutSynth  : public AudioProcessor
{
public:
    enum
    {
        numInputBuses = 1,
        numOutputBuses = NUM_OUT_BUSES,
        numSynths = 7,
        numVoices = 4
    };

    MultiOutSynth();

    bool isBusesLayoutSupported(const BusesLayout&) const override;

    void prepareToPlay(double newSampleRate, int samplesPerBlock) override;
    void releaseResources() override                       {}
    void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiBuffer) override;

    AudioProcessorEditor* createEditor() override          { return new GenericAudioProcessorEditor (*this); }
    bool hasEditor() const override                        { return true; }

    const String getName() const override                  { return "Multi Out Synth PlugIn"; }
    bool acceptsMidi() const override                      { return false; }
    bool producesMidi() const override                     { return false; }
    double getTailLengthSeconds() const override           { return 0; }
    int getNumPrograms() override                          { return 1; }
    int getCurrentProgram() override                       { return 0; }
    void setCurrentProgram (int) override                  {}
    const String getProgramName (int) override             { return {}; }
    void changeProgramName (int, const String&) override   {}

    void getStateInformation(MemoryBlock& mb) override     { mb.setSize(1); }
    void setStateInformation (const void*, int) override   {}

private:
    static AudioProcessor::BusesProperties buildBusesProperties();

    OwnedArray<Synthesiser> synth;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultiOutSynth)
};
