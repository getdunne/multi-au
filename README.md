# multi-AU
**tl;dr: The answer is "change the version number".** See bottom of page.

I have a commercial plug-in product, for which I would like to offer a paid upgrade from the present stereo-only version (single stereo output bus) to a multi-output version, **without changing the AU subtype code** (so DAW projects saved with the earlier version will continue to load with the upgraded version).

For the Audio-Unit plug-in, this seems to be impossible under MacOS 10.15.6 "Catalina", due to some unexplained persistence of the Audio Unit cache.

This repo contains the simplest code example I could prepare, to reproduce this issue.

## The example code

[JUCE](http://juce.com) provides straightforward mechanisms to define the number and configuration of input-output buses for a plug-in, which are illustrated by this simple example. The code is based on the [JUCE Multi-Out Synth Plug-in tutorial](https://docs.juce.com/master/tutorial_plugin_examples.html#tutorial_plugin_examples_multi_out_synth), with three main changes:
1. I have replaced the *SamplerVoice* instances with a new *SineWaveVoice* class implementing sine-wave oscillators (based on the [JUCE sine-wave synthesizer](https://docs.juce.com/master/tutorial_sine_synth.html) tutorial code).
2. In the *MultiOutSynth* constructor, I have replaced the static *.withOutput()* invocations with a new static function called *buildBusesProperties()* which adds one input bus, and one or more output buses, according to the value of a variable *numOutputBuses*, initialized from a *#define* set in each of the *.jucer* files as either 1 or 16.
3. I made appropriate changes to the *isBusesLayoutSupported()* implementation, so it accepts only bus layouts compatible with what was set up by *buildBusesProperties()*.

The same source code is referenced by all three *.jucer* projects, which differ only in how they set the *#define NUM_OUT_BUSES* constant, the project name, and the Audio Unit subtype (*Sout* or *Mout*):
- *Single-Sout.jucer* sets *NUM_OUT_BUSES=1* and AU subtype *Sout*.
- *Multi-Mout.jucer* sets *NUM_OUT_BUSES=16* and AU subtype *Mout*.
- *Multi-Sout.jucer* sets *NUM_OUT_BUSES=16* and AU subtype *Sout*.

## Compiled binaries

The *Binaries* folder includes two compiled versions of the *Single-Sout* and *Multi-Sout* plug-ins.
 - *Debug-Version.zip* contains Debug builds, compiled with Xcode set for Development Signing
 - *Release-Notarized.zip* contains Release builds, fully signed and notarized.
 - *Release-Notarized-2in8out.zip* contains a version of *Multi-Sout* built for 2 stereo input buses and 8 stereo output buses.
 - *SingleSout-VersionUpdate.zip* contains three *SingleSout* builds with different version numbers.
 
I wanted to see if code-signing and/or notarization would make a difference; it did not.

What DID make a difference--all the difference in the world--was structuring the change as a standard version update. I compiled *SingleSout* with version "1.1" in 1-in, 1-out configuration, then again with version "2.0" in 1-in, 16-out configuration. I could then go back and forth between the two, and each time *Logic Pro X* (10.5.1, under *Catalina*) correctly detected the change. I then compiled the 1-in, 16-out configuration as version "1.2", just to check if there was anything special about the "major" version number; it appears there is not.

## Reproducing the issue

If you build the first two Audio Unit plug-ins (or use the prebuilt binaries), and install them on a "virgin" MacOS system, *Logic Pro X* will correctly identify that the *Single-Sout* plug-in supports only stereo output, and the *Multi-Mout* plug-in supports "stereo" and "16x stereo". (Logic does not present all possible options.)

However, if you then build and install the *Multi-Sout* plug-in, and remove *Single-Sout* (so there is still only one plug-in with AU subtype "Sout") Logic Pro remembers that the AU subtype code "Sout" is associated with one stereo output bus only, and I have not been able to find any way to reset whatever caching mechanism it uses, so it can recognize that the plug-in is now capable of supporting up to 16 stereo output buses. Things I have tried include:
 - Log out / log in
 - Restart MacOS
 - *sudo killall -9 AudioComponentRegistrar* on the command line
 - Delete *~/Library/Caches/AudioUnitCache*
 - Delete *~/Library/Caches/com.apple.logic10*
 - Delete *~/Library/Caches/com.apple.musicapps*

The above measures appear to WORK with MacOS 10.13.6 and Logic Pro X 10.4.8, but FAIL under MacOS 10.15.6 and Logic Pro X 10.5.1.
 
## UPDATE: The answer is "change the version number"!

Following a suggestion from Apple, I tested the same steps using Logic Pro 10.4.8 on MacOS 10.15.6 Catalina, and **this works as expected**. It is even possible to get Logic 10.4.8 to show the new I/O options by simply selecting the *Single-Sout* plug-in in the Plug-In Manager and choosing "Reset and Rescan Selection"; it's not even necessary to delete any caches. (If you do this, Logic will continue to use the old plug-in name "Single-Sout", but it will correctly load the the "Multi-Sout" plug-in.)

According to my Apple contact, the issue is not *Logic Pro* itself, but simply the fact that Logic 10.5.1 is the first to use "an OS service" (presumably *AudioComponentRegistrar*) to scan plug-ins. As my final set of experiments proved, **AudioComponentRegistrar requires a version-number change to trigger correct re-scanning**, and *it works just as well whether the version number increases or decreases* (so downgrades are perfectly possible).

In hindsight, this is hardly surprising, but I greatly regret that it took more than a week of extra work, and imposing on the time and goodwill of a friendly Apple insider, to learn this tiny but critical detail.

