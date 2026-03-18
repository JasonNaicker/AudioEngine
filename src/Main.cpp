#include "AudioConfig.h"
#include "AudioTypes.h"
#include "AudioStreamer.h"
#include <iostream>
#include <cmath>
#include <numbers>
#include <vector>
#include <fstream>
#include <filesystem>

int main()
{
    std::ifstream file("C:/Code/CppPrograms/AudioStreamer/build/billie_jean.pcm", std::ios::binary);
    size_t fileSize = std::filesystem::file_size("C:/Code/CppPrograms/AudioStreamer/build/billie_jean.pcm");
    std::vector<Sample> inputBuffer(fileSize / sizeof(Sample));
    file.read((char*)inputBuffer.data(), fileSize);
    file.close();

    // pad
    size_t remainder = inputBuffer.size() % AudioConfig::SAMPLE_SIZE;
    if (remainder != 0) {
        inputBuffer.resize(inputBuffer.size() + (AudioConfig::SAMPLE_SIZE - remainder), 0);
    }
 
    AudioStreamer audioStream(std::span<const Sample>{inputBuffer}, "C:/Code/CppPrograms/AudioStreamer/build/output.wav");
    audioStream.Start();
    std::this_thread::sleep_for(std::chrono::seconds(300)); // 5 minutes
    audioStream.Stop();

    return 0;
}