#pragma once
#include "miniaudio.h"
#include "AudioTypes.h"
#include "MixConfig.h"
#include "AudioConfig.h"
#include <span>

class AudioMixer {
public:
    static void processGain(std::span<Sample> batch, float* right);
    static void compressChannels(std::span<Sample> batch);

    static void lowPassFilter(std::span<Sample> batch, float cutoff);
    static void highPassFilter(std::span<Sample> batch, float cutoff);

    static void pan(std::span<Sample> batch, float pan);

    static void delay(std::span<Sample> batch, int delaySamples, float feedback);

    static void tremolo(std::span<Sample> batch, float rate, float depth);

    static void distortion(std::span<Sample> batch, float drive);

    static void reverse(std::span<Sample> batch);
    static void removeDCOffset(std::span<Sample> batch);

    static void normalize(std::span<Sample> batch);

    static void softClip(std::span<Sample> batch, float threshold);

    static void invertPolarity(std::span<Sample> batch);

    static void resample(std::span<Sample> batch, int originalSampleRate, int targetSampleRate);

    static void simpleEcho(std::span<Sample> batch, int delaySamples, float feedback);

    static void applyADSR(std::span<Sample> batch, float attack, float decay, float sustain, float release);

    static void applyLFO(std::span<Sample> batch, float rate, float depth);

    static void reverb(std::span<Sample> batch, int roomSize, float feedback);
};