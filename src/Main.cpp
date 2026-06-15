#include "AudioConfig.h"
#include "AudioTypes.h"
#include "AudioFile.h"
#include "AudioStreamer.h"
#include "Wav.h"

#include <iostream>
#include <cmath>
#include <thread>
#include <numbers>
#include <vector>
#include <fstream>
#include <filesystem>

static std::vector<Sample> generateSine(float freq, float durationSec, int sampleRate, int channels) {
    std::vector<Sample> out;

    const size_t totalFrames = static_cast<size_t>(durationSec * sampleRate);

    out.resize(totalFrames * channels);

    const float twoPiF = 2.0f * std::numbers::pi_v<float> * freq;

    for (size_t i = 0; i < totalFrames; ++i) {
        float t = static_cast<float>(i) / sampleRate;
        float value = std::sin(twoPiF * t);

        for (int c = 0; c < channels; ++c) {
            out[i * channels + c] = value;
        }
    }

    return out;
}

int main()
{
    const std::string inputPath =
        "C:/Code/CppPrograms/AudioStreamer/inputAudio/pcm/human_nature_44_f32.pcm";

    const std::string outputPath =
        "C:/Code/CppPrograms/AudioStreamer/build/output.wav";

    const std::string pcmOut =
        "C:/Code/CppPrograms/AudioStreamer/build/test_output.pcm";

    const std::string wavOut =
        "C:/Code/CppPrograms/AudioStreamer/build/test_output.wav";

    const AudioFormatInfo infoFormat = {
        ma_format_s16,
        AudioConfig::CHANNELS,
        44100
    };


    std::vector<Sample> inputBuffer = AudioFile::load(inputPath, infoFormat).buffer;
    std::vector<Sample> saveBuffer = generateSine(440.0f,10, AudioConfig::SAMPLE_RATE, AudioConfig::CHANNELS);

    //AudioFile::save(wavOut, std::span<const Sample>{saveBuffer}, infoFormat);
    //AudioFile::save(pcmOut, std::span<const Sample>{saveBuffer}, infoFormat);
    
    AudioStreamer audioStream(std::span<const Sample>{inputBuffer}, false, outputPath);

    audioStream.Start();

    std::this_thread::sleep_for(std::chrono::seconds(10));

    audioStream.Pause();

    std::this_thread::sleep_for(std::chrono::seconds(2));

    audioStream.Resume();

    std::this_thread::sleep_for(std::chrono::seconds(300));

    audioStream.Stop();

    return 0;
}