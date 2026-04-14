#pragma once
#include "miniaudio.h"
#include "AudioTypes.h"
#include "AudioBalance.h"
#include "MixConfig.h"
#include "AudioConfig.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <span>
#include <ranges>
#include <numeric>
#include <immintrin.h>
#include <xsimd/xsimd.hpp>

class AudioMixer {
public:
    // =========================
    // Core Gain / Amplitude
    // =========================
    static void processGain(std::span<Sample>& batch, float* right, AudioBalanceMode mode);

    static void normalize(std::span<Sample>& batch, AudioBalanceMode) {
        if (batch.empty()) return;
    
        constexpr size_t size = SimdBatch::size;
        SimdBatch maxBatch = SimdBatch(0.0f);

        for(size_t i = 0; i + size <= batch.size(); i += size) {
            _mm_prefetch((const char*) (batch.data() + i + PREFETCH_AMOUNT * size), _MM_HINT_T0);
            SimdBatch currentBatch = xsimd::load_aligned(batch.data() + i);
            maxBatch = xsimd::max(maxBatch, xsimd::abs(currentBatch));
        }
        Sample maxVal = xsimd::reduce_max(maxBatch); //Horizontal reduction

        if(maxVal <= Sample(0)) return;

        const SimdBatch batchGain(maxVal);
        const SimdBatch invGain = SimdBatch(Sample(1)) / batchGain;

        for(size_t i = 0; i + size <= batch.size(); i += size) {
            SimdBatch currentBatch = xsimd::load_aligned(batch.data() + i);
            currentBatch *= invGain;
            currentBatch.store_aligned(batch.data() + i);
        }
    }

    static void invertPolarity(std::span<Sample>& batch, AudioBalanceMode) {
        if (batch.empty()) return;
    
        constexpr size_t size = SimdBatch::size;

        for(size_t i = 0; i + size <= batch.size(); i += size) {
            _mm_prefetch((const char*) (batch.data() + i + PREFETCH_AMOUNT * size), _MM_HINT_T0);
            SimdBatch currentBatch = xsimd::load_aligned(batch.data() + i);
            currentBatch *= Sample(-1);
            currentBatch.store_aligned(batch.data() + i);
        }
    }

    static void removeDCOffset(std::span<Sample>& batch) {
        if (batch.empty()) return;
    
        constexpr size_t size = SimdBatch::size;
        float avg = 0.0f;
        const float EPSILON = 1e-6f;

        for(size_t i = 0; i + size < batch.size(); i += size) {
            _mm_prefetch((const char*) (batch.data() + i + PREFETCH_AMOUNT * size), _MM_HINT_T0);
            SimdBatch currentBatch = xsimd::load_unaligned(batch.data() + i);
            avg += xsimd::reduce_add(currentBatch);
        }
        avg /= batch.size();
        if(fabs(avg) < EPSILON) return;

        for(size_t i = 0; i + size < batch.size(); i += size) {
            SimdBatch currentBatch = xsimd::load_unaligned(batch.data() + i);
            currentBatch -= avg;
            Sample(currentBatch[i]);
            currentBatch.store_aligned(batch.data() + i);
        }
    }

    // =========================
    // Dynamics
    // =========================
    static void compressChannels(std::span<Sample>& batch, AudioBalanceMode mode);
    static void limiter(std::span<Sample> batch, float threshold, float attack, float release);

    // =========================
    // Filters / EQ
    // =========================
    static void lowPassFilter(std::span<Sample>& batch, float cutoff, AudioBalanceMode mode);
    static void highPassFilter(std::span<Sample>& batch, float cutoff, AudioBalanceMode mode);
    static void biquad(std::span<Sample>& batch, float b0, float b1, float b2, float a1, float a2, AudioBalanceMode mode);
    static void parametricEQ(std::span<Sample>& batch, float freq, float q, float gainDB, AudioBalanceMode mode);

    // =========================
    // Spatial / Stereo
    // =========================
    static void pan(std::span<Sample>& batch, float pan, AudioBalanceMode mode);
    static void midSideEncode(std::span<Sample> batch);
    static void midSideDecode(std::span<Sample> batch);

    // =========================
    // Time-Based Effects
    // =========================
    static void delay(std::span<Sample>& batch, int delaySamples, float feedback, AudioBalanceMode mode);
    static void simpleEcho(std::span<Sample>& batch, int delaySamples, float feedback, AudioBalanceMode mode);
    static void reverb(std::span<Sample>& batch, int roomSize, float feedback, AudioBalanceMode mode);

    // =========================
    // Modulation Effects
    // =========================
    static void tremolo(std::span<Sample>& batch, float rate, float depth, AudioBalanceMode mode);
    static void applyLFO(std::span<Sample>& batch, float rate, float depth, AudioBalanceMode mode);
    static void chorus(std::span<Sample>& batch, float rate, float depth, AudioBalanceMode mode);
    static void flanger(std::span<Sample>& batch, float rate, float depth, float feedback, AudioBalanceMode mode);
    static void phaser(std::span<Sample>& batch, float rate, float depth, AudioBalanceMode mode);

    // =========================
    // Nonlinear / Tone Shaping
    // =========================
    static void distortion(std::span<Sample>& batch, float drive, AudioBalanceMode mode);
    static void softClip(std::span<Sample>& batch, float threshold, AudioBalanceMode mode);
    // =========================
    // Envelope / Control
    // =========================
    static void applyADSR(std::span<Sample>& batch, float attack, float decay, float sustain, float release, AudioBalanceMode mode);

    // =========================
    // Buffer Operations
    // =========================
    static void reverse(std::span<Sample>& batch, AudioBalanceMode mode);
    static void resample(std::span<Sample>& batch, int originalSampleRate, int targetSampleRate, AudioBalanceMode mode);
    static void copy(std::span<Sample> src, std::span<Sample> dst);
    static void interleave(std::span<Sample>& left, std::span<Sample> right, std::span<Sample> out);
    static void deinterleave(std::span<Sample>& in, std::span<Sample> left, std::span<Sample> right);

    // =========================
    // Mixing / Blending
    // =========================
    template<typename Func>
    static void mix(std::span<Sample> a, std::span<Sample> b, Func f) {
        size_t n = std::min(a.size(), b.size());
        for (size_t i = 0; i < n; ++i) {
            a[i] = f(a[i], b[i]);
        }
    }

    static void crossfade(std::span<Sample>& a, std::span<Sample>& b, float t);
    static void blend(std::span<Sample>& dst, std::span<Sample>& src, float gain);

    // =========================
    // Analysis / Metering
    // =========================
    static float rms(std::span<Sample>& batch);
    static float peak(std::span<Sample>& batch);
    static int zeroCrossings(std::span<Sample>& batch);

    template<typename Func>
    inline static float accumulate(std::span<Sample>& batch, Func f) {
        float result = 0.0f;
        for (auto& s : batch) {
            result = f(result, s);
        }
        return result;
    }

    // =========================
    // Generic Processing (HOFs)
    // =========================
    template<typename Func>
    inline static void apply(std::span<Sample>& batch, Func f) {
        constexpr size_t size = SimdBatch::size;
        for(size_t i = 0; i + size <= batch.size(); i += size) {
            _mm_prefetch((const char*) (batch.data() + i + PREFETCH_AMOUNT * size), _MM_HINT_T0);
            SimdBatch currentBatch = xsimd::load_unaligned(batch.data() + i);
            f(currentBatch);
            currentBatch.store_aligned(batch.size() + i);
        }
    }

    template<typename Func>
    inline static void processInPlace(std::span<Sample>& batch, Func f) {
        for (auto& s : batch) {
            s = f(s);
        }
    }

    template<typename Func>
    inline static void processToBuffer(std::span<Sample>& in, std::span<Sample>& out, Func f) {
        size_t n = std::min(in.size(), out.size());
        for (size_t i = 0; i < n; ++i) {
            out[i] = f(in[i]);
        }
    }

    template<typename... Funcs>
    inline static void chain(std::span<Sample>& batch, Funcs... funcs) {
        (apply(batch, funcs), ...);
    }

    inline static void convolve(std::span<Sample>& input, std::span<Sample> impulse) {
        if (input.empty() || impulse.empty()) return;

        std::vector<Sample> result(input.size(), 0.0f);

        for (size_t i = 0; i < input.size(); ++i) {
            for (size_t j = 0; j < impulse.size(); ++j) {
                if (i >= j)
                    result[i] += input[i - j] * impulse[j];
            }
        }

        std::copy(result.begin(), result.end(), input.begin());
    }
private:
    static const size_t PREFETCH_AMOUNT = 4;
};