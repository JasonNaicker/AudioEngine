#pragma once
#include <cstddef>

namespace AudioConfig {

constexpr std::size_t CHANNELS = 2; //Stereo
constexpr std::size_t FRAME_SIZE = 1024; //Size of batches
constexpr std::size_t SAMPLE_SIZE = FRAME_SIZE * CHANNELS; //Including channels

constexpr std::size_t BUFFER_SIZE = 8192; //Total buffer size
constexpr std::size_t SAMPLE_RATE = 48000; //48000 hz (frames per seccond)

}