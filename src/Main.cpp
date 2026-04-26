#include "AudioConfig.h"
#include "AudioTypes.h"
#include "AudioFile.h"
#include "AudioStreamer.h"
#include <iostream>
#include <cmath>
#include <thread>
#include <numbers>
#include <vector>
#include <fstream>
#include <filesystem>

int main()
{
    const std::string inputPath = "C:/Code/CppPrograms/AudioStreamer/inputAudio/pcm/beat_it_48_f32.pcm";
    const std::string outputPath = "C:/Code/CppPrograms/AudioStreamer/build/output.wav";


    const AudioFormatInfo info = {
        ma_format_f32,
        AudioConfig::CHANNELS,
        AudioConfig::SAMPLE_RATE
    };
     std::vector<Sample> inputBuffer = AudioFile::load(inputPath, info).buffer;
     const AudioFormatInfo test = {
        ma_format_f32,
        AudioConfig::CHANNELS,
        AudioConfig::SAMPLE_RATE
    };

    inputBuffer = AudioFile::reformat(inputBuffer, info, test);
    AudioStreamer audioStream(std::span<const Sample>{inputBuffer}, false, outputPath);
    audioStream.Start();
    std::this_thread::sleep_for(std::chrono::seconds(10));
    audioStream.Pause();
    std::this_thread::sleep_for(std::chrono::seconds(10));
    audioStream.Resume();
    std::this_thread::sleep_for(std::chrono::seconds(300)); // 5 minutes
    audioStream.Stop();

    return 0;
}