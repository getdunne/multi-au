#include "MultiOutSynth.h"
#include "SineWaveVoice.h"

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MultiOutSynth();
}


MultiOutSynth::MultiOutSynth() : AudioProcessor(buildBusesProperties())
{
    int noteOffsets[] = { 0, 12, 7, 4, -12, -5, -8 };
    double gain = getBusCount(false) == 1 ? 0.25 : 1.0;

    for (int si = 0; si < numSynths; ++si)
    {
        synth.add(new Synthesiser())->addSound(new SineWaveSound);

        for (int i = 0; i < numVoices; ++i)
            synth[si]->addVoice(new SineWaveVoice(noteOffsets[si], gain));
    }
}

AudioProcessor::BusesProperties MultiOutSynth::buildBusesProperties()
{
    BusesProperties busesProperties;

    busesProperties.addBus(true, "Main Input", AudioChannelSet::stereo(), false);
    
    busesProperties.addBus(false, "Main Output", AudioChannelSet::stereo(), true);
    for (int i = 1; i < numOutputBuses; i++)
        busesProperties.addBus(false, "Aux " + String(i), AudioChannelSet::stereo(), false);

    return busesProperties;
}

bool MultiOutSynth::isBusesLayoutSupported(const BusesLayout& layout) const
{
    // One input bus: stereo or disabled
    if (layout.inputBuses.size() != numInputBuses) return false;
    for (auto& ib : layout.inputBuses)
        if (ib != AudioChannelSet::stereo() && !ib.isDisabled()) return false;
    
    // Exactly our specified number of output buses, stereo or disabled
    if (layout.outputBuses.size() != numOutputBuses) return false;
    for (auto& ob : layout.outputBuses)
        if (ob != AudioChannelSet::stereo() && !ob.isDisabled()) return false;

    return true;
}

void MultiOutSynth::prepareToPlay(double newSampleRate, int /*samplesPerBlock*/)
{
    auto numEnabledBuses = 0;
    auto const& layout = getBusesLayout();
    for (auto& ob : layout.outputBuses)
        numEnabledBuses += ob.isDisabled() ? 0 : 1;
    DBG (String(layout.outputBuses.size()) + " output buses, " + String(numEnabledBuses) + " enabled");
    
    for (auto si = 0; si < numSynths; ++si)
        synth[si]->setCurrentPlaybackSampleRate(newSampleRate);
}

void MultiOutSynth::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiBuffer)
{
    int numSamples = buffer.getNumSamples();
    bool inputBusExists = getBusCount(true) > 0;
    jassert(inputBusExists);

    auto busCount = getBusCount(false);
    if (busCount == 1)
    {
        // Stereo-only case: all synths render to main output bus
        buffer.clear();
        for (int si = 0; si < numSynths; si++)
            synth[si]->renderNextBlock(buffer, midiBuffer, 0, numSamples);
    }
    else
    {
        // Multi-output case: each synth renders to its corresponding output bus
        for (int iBus = 0; iBus < busCount; ++iBus)
        {
            auto outputBusBuffer = getBusBuffer(buffer, false, iBus);
            outputBusBuffer.clear();

            if (iBus < numSynths)
                synth[iBus]->renderNextBlock(outputBusBuffer, midiBuffer, 0, numSamples);
        }
    }
}
