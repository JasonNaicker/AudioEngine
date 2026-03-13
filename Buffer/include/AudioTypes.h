#pragma once

#include <cstdint>
#include "CircularBuffer.h"
#include "AudioConfig.h"

using Sample = int16_t;

using AudioBuffer = CircularBuffer<Sample, AudioConfig::SAMPLE_SIZE>;