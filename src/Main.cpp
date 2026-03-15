#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include "AudioConfig.h"
#include "AudioTypes.h"
#include "AudioStreamer.h"
#include <iostream>
#include <cmath>
#include <vector>

int main()
{
    constexpr float FREQUENCY = 440.0f;
    constexpr float AMPLITUDE = 0.5f;

    std::vector<Sample> sample(AudioConfig::SAMPLE_SIZE * AudioConfig::SAMPLE_RATE); //1 second

    for (size_t i = 0; i < AudioConfig::FRAME_SIZE * AudioConfig::SAMPLE_RATE; i++)
    {
        float t = float(i) / AudioConfig::SAMPLE_RATE;
        float vLeft = AMPLITUDE * std::sin(2.0 * M_PI * FREQUENCY * t);
        float vRight = AMPLITUDE * std::sin(2.0 * M_PI * FREQUENCY * t + M_PI / 2);

        sample[i * AudioConfig::CHANNELS] = static_cast<int16_t>(vLeft * 32767);
        sample[i * AudioConfig::CHANNELS + 1] = static_cast<int16_t>(vRight * 32767);
    }
    AudioStreamer audioStream(std::span<const Sample>{sample});
    audioStream.Start();

    std::this_thread::sleep_for(std::chrono::seconds(5));

    audioStream.Stop();

    return 0;
}