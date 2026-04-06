#pragma once
#include "miniaudio.h"
#include "AudioTypes.h"
#include "AudioBalance.h"
#include "MixConfig.h"
#include "AudioConfig.h"
#include <span>

class AudioMixer {
public:
    static void processGain(std::span<Sample> batch, float* right, AudioBalanceMode mode);
    static void compressChannels(std::span<Sample> batch, AudioBalanceMode mode);

    static void lowPassFilter(std::span<Sample> batch, float cutoff, AudioBalanceMode mode);
    static void highPassFilter(std::span<Sample> batch, float cutoff, AudioBalanceMode mode);

    static void pan(std::span<Sample> batch, float pan, AudioBalanceMode mode);

    static void delay(std::span<Sample> batch, int delaySamples, float feedback, AudioBalanceMode mode);

    static void tremolo(std::span<Sample> batch, float rate, float depth, AudioBalanceMode mode);

    static void distortion(std::span<Sample> batch, float drive, AudioBalanceMode mode);

    static void reverse(std::span<Sample> batch, AudioBalanceMode mode);
    static void removeDCOffset(std::span<Sample> batch);

    static void normalize(std::span<Sample> batch, AudioBalanceMode mode);

    static void softClip(std::span<Sample> batch, float threshold, AudioBalanceMode mode);

    static void invertPolarity(std::span<Sample> batch, AudioBalanceMode mode);

    static void resample(std::span<Sample> batch, int originalSampleRate, int targetSampleRate, AudioBalanceMode mode);

    static void simpleEcho(std::span<Sample> batch, int delaySamples, float feedback, AudioBalanceMode mode);

    static void applyADSR(std::span<Sample> batch, float attack, float decay, float sustain, float release, AudioBalanceMode mode);

    static void applyLFO(std::span<Sample> batch, float rate, float depth, AudioBalanceMode mode);

    static void reverb(std::span<Sample> batch, int roomSize, float feedback, AudioBalanceMode mode);
};
