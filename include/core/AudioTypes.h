#pragma once
#include <cstdint>
#include "CircularBuffer.h"
#include "AudioConfig.h"
#include <xsimd/xsimd.hpp>

using Sample = float; //Internal bit depth
using SampleSaved = int16_t; //Saved file bit depth
using AudioBuffer = CircularBuffer<Sample, AudioConfig::SAMPLE_SIZE>;
using SimdBatch = xsimd::batch<Sample>;