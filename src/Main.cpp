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
    std::string inputPath = "C:/Code/CppPrograms/AudioStreamer/build/billie_jean_f32.pcm";
    std::string outputPath = "C:/Code/CppPrograms/AudioStreamer/build/output.wav";
    std::vector<Sample> inputBuffer = AudioFile::load(inputPath);
    /*
    if(file.is_open()) {
        size_t fileSize = std::filesystem::file_size(inputPath);
        inputBuffer = std::vector<Sample>(fileSize / sizeof(Sample));
        file.read((char*) inputBuffer.data(), fileSize);
        file.close();
    }

    // pad
    size_t remainder = inputBuffer.size() % AudioConfig::SAMPLE_SIZE;
    if (remainder != 0) {
        inputBuffer.resize(inputBuffer.size() + (AudioConfig::SAMPLE_SIZE - remainder), 0);
    }
 */

    AudioStreamer audioStream(std::span<const Sample>{inputBuffer}, true, outputPath);
    audioStream.Start();
    std::this_thread::sleep_for(std::chrono::seconds(300)); // 5 minutes
    audioStream.Stop();

    return 0;
}