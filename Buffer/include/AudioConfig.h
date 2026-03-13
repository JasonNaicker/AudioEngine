#pragma once
#include <cstddef>

namespace AudioConfig {

constexpr std::size_t CHANNELS = 2;
constexpr std::size_t FRAME_SIZE = 1024;
constexpr std::size_t SAMPLE_SIZE = FRAME_SIZE * CHANNELS;

constexpr std::size_t BUFFER_SIZE = 8192;
constexpr std::size_t SAMPLE_RATE = 48000;

}