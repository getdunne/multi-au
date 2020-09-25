# multi-AU
[JUCE](http://juce.com) provides straightforward mechanisms to define the number and configuration of input-output buses for a plug-in, which are illustrated by this simple example. The code is based on the [JUCE Multi-Out Synth Plug-in tutorial](https://docs.juce.com/master/tutorial_plugin_examples.html#tutorial_plugin_examples_multi_out_synth), with three main changes:
1. I have replaced the *SamplerVoice* instances with a new *SineWaveVoice* class implementing sine-wave oscillators (based on the [JUCE sine-wave synthesizer](https://docs.juce.com/master/tutorial_sine_synth.html) tutorial code).
2. In the *MultiOutSynth* constructor, I have replaced the static *.withOutput()* invocations with a new static function called *buildBusesProperties()* which adds one input bus, and one or more output buses, according to the value of a variable *numOutputBuses*, initialized from a *#define* set in each of the *.jucer* files as either 1 or 16.
3. I made appropriate changes to the *isBusesLayoutSupported()* implementation, so it accepts only bus layouts compatible with what was set up by *buildBusesProperties()*.

The same source code is referenced by all three *.jucer* projects, which differ only in how they set the *#define NUM_OUT_BUSES* constant, the project name, and the Audio Unit subtype (*Sout* or *Mout*):
- *Single-Sout.jucer* sets *NUM_OUT_BUSES=1* and AU subtype *Sout*.
- *Multi-Mout.jucer* sets *NUM_OUT_BUSES=16* and AU subtype *Mout*.
- *Multi-Sout.jucer* sets *NUM_OUT_BUSES=16* and AU subtype *Sout*.

If you build the first two Audio Unit plug-ins, and install them on a "virgin" MacOS system, *Logic Pro X* will correctly identify that the *Single-Sout* plug-in supports only stereo output, and the *Multi-Mout* plug-in supports "stereo" and "16x stereo". (Logic does not present all possible options.)

However, if you then build and install the *Multi-Sout* plug-in, and remove *Single-Sout* (so there is still only one plug-in with AU subtype "Sout") Logic Pro remembers that the AU subtype code "Sout" is associated with one stereo output bus only, and I have not been able to find any way to reset whatever caching mechanism it uses, so it can recognize that the plug-in is now capable of supporting up to 16 stereo output buses. Things I have tried include:
 - Log out / log in
 - Restart MacOS
 - *sudo killall -9 AudioComponentRegistrar* on the command line
 - Delete *~/Library/Caches/AudioUnitCache*
 - Delete *~/Library/Caches/com.apple.logic10*
 - Delete *~/Library/Caches/com.apple.musicapps*

The above measures appear to WORK with MacOS 10.13.6 and Logic Pro X 10.4.8, but FAIL under MacOS 10.15.6 and Logic Pro X 10.5.1.

## Why this is important
Replacing the *Single-Sout* plug-in with *Multi-Sout* models the situation where a plug-in product is upgraded (perhaps as a paid upgrade) from a single-bus, stereo-only version, to a newer/better version which supports multiple output buses.

It is highly desirable to retain the original AU subtype code ("Sout" in this case), so Logic Pro X projects saved using the earlier version of the plug-in (identified by the subtype code) will still load correctly using the newer version, but this seems to be impossible, at least under MacOS 10.15.6 "Catalina".
