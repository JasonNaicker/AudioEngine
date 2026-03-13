#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include "AudioTypes.h"
#include "AudioStreamer.h"
#include <iostream>
#include <cmath>
#include "AudioConfig.h"

int main()
{
    AudioStreamer audioStream;

    constexpr float FREQUENCY = 440.0f;
    constexpr float AMPLITUDE = 0.5f;

    Sample sample[AudioConfig::FRAME_SIZE];

    for (size_t i = 0; i < AudioConfig::FRAME_SIZE; i++)
    {
        float t = float(i) / AudioConfig::SAMPLE_RATE;
        float v = AMPLITUDE * std::sin(2.0 * M_PI * FREQUENCY * t);

        sample[i] = static_cast<int16_t>(v * 32767);

        std::cout << v << "\n";
    }

    return 0;
}