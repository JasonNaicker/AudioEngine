#pragma once
#include "miniaudio.h"
#include <cstdint>
#include "CircularBuffer.h"
#include "AudioConfig.h"
#include <xsimd/xsimd.hpp>

using Sample = float; //Internal bit depth
using SampleSaved = int16_t; //Saved file bit depth
using AudioBuffer = CircularBuffer<Sample, AudioConfig::SAMPLE_SIZE>;
using SimdBatch = xsimd::batch<Sample>;
struct AudioFormatInfo {
    ma_format format = ma_format_f32;
    ma_uint32 channels = AudioConfig::CHANNELS;
    ma_uint32 sampleRate = AudioConfig::SAMPLE_RATE;
};

struct AudioData {
    std::vector<Sample> buffer;
    AudioFormatInfo sourceFormat;
    AudioFormatInfo runtimeFormat;
};