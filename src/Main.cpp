#include "AudioConfig.h"
#include "AudioTypes.h"
#include "AudioStreamer.h"
#include <iostream>
#include <cmath>
#include <numbers>
#include <vector>

int main()
{
    constexpr float FREQUENCY = 440.0f;
    constexpr float AMPLITUDE = 0.5f;
    constexpr int DURATION = 120; //Duration in seconds
    std::vector<Sample> inputBuffer(AudioConfig::SAMPLE_RATE * AudioConfig::CHANNELS * DURATION); //1 second
    float phaseStep = 2.0f * std::numbers::pi_v<float> * FREQUENCY / AudioConfig::SAMPLE_RATE;
    //Create input
    for (size_t i = 0; i < AudioConfig::SAMPLE_RATE; i++)
    {
        float phase = phaseStep * i;
        float vLeft = AMPLITUDE * std::sin(phase);
        float vRight = AMPLITUDE * std::sin(phase + std::numbers::pi_v<float> / 2);

        inputBuffer[i * AudioConfig::CHANNELS] = static_cast<int16_t>(vLeft * 32767);
        inputBuffer[i * AudioConfig::CHANNELS + 1] = static_cast<int16_t>(vRight * 32767);
    }

    //Pad input
    size_t remainder = inputBuffer.size() % AudioConfig::SAMPLE_SIZE;
    if(remainder != 0) {
        inputBuffer.resize(inputBuffer.size() + (AudioConfig::SAMPLE_SIZE - remainder), 0);
    }
    AudioStreamer audioStream(std::span<const Sample>{inputBuffer});
    audioStream.Start();

    std::this_thread::sleep_for(std::chrono::seconds(15));

    audioStream.Stop();

    return 0;
}