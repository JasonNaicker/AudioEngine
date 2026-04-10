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
    std::string inputPath = "C:/Code/CppPrograms/AudioStreamer/inputAudio/pcm/beat_it_48_f32.pcm";
    std::string outputPath = "C:/Code/CppPrograms/AudioStreamer/build/output.wav";
    const std::vector<Sample> inputBuffer = AudioFile::load(inputPath);

    AudioStreamer audioStream(std::span<const Sample>{inputBuffer}, false, outputPath);
    audioStream.Start();
    std::this_thread::sleep_for(std::chrono::seconds(300)); // 5 minutes
    audioStream.Stop();

    return 0;
}