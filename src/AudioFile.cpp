#include "AudioFile.h"
#include "AudioConfig.h"
#include "AudioTypes.h"
#include "Wav.h"
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

std::vector<Sample> AudioFile::loadPCM(const std::string& path) {
    if(path.empty()) throw std::runtime_error("Path is empty");

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) throw std::runtime_error("Failed to load: " + path);

    size_t fileSize = std::filesystem::file_size(path);
    std::vector<Sample> inputBuffer(fileSize / sizeof(Sample));
    file.read((char*) inputBuffer.data(), fileSize);
    return inputBuffer;

}

std::vector<Sample> AudioFile::loadWAV(const std::string& path) {
    if(path.empty()) throw std::runtime_error("Path is empty");

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) throw std::runtime_error("Failed to load: " + path);
    
    size_t fileSize = std::filesystem::file_size(path);
    size_t dataSize = fileSize - 44; //Header size (44 bytes)

    file.seekg(44);
    std::vector<Sample> inputBuffer(dataSize / sizeof(Sample));
    file.read((char*) inputBuffer.data(), dataSize);
    return inputBuffer;
}

std::vector<Sample> AudioFile::loadMP3(const std::string& path) {
    throw std::runtime_error("MP3 loading not yet implemented");
}

std::vector<Sample> AudioFile::load(const std::string& path) {
    std::vector<Sample> inputBuffer;
    if(path.ends_with(".pcm")) inputBuffer = loadPCM(path);
    else if(path.ends_with(".wav")) inputBuffer = loadWAV(path);
    else if(path.ends_with(".mp3")) inputBuffer = loadMP3(path);
    else throw std::runtime_error("Unsupported format " + path);
    pad(inputBuffer);
    return inputBuffer;
}

void AudioFile::pad(std::vector<Sample>& buffer) {
    size_t remainder = buffer.size() % AudioConfig::SAMPLE_SIZE;
    if (remainder != 0) {
        buffer.resize(buffer.size() + (AudioConfig::SAMPLE_SIZE - remainder), 0);
    }
}

void AudioFile::open(const std::string& path) {
    if(path.empty()) return;
    outputFile.open(path, std::ios::binary);

    if (!outputFile.is_open()) throw std::runtime_error("Failed to write to: " + path);

    if(path.ends_with(".wav")) {
        format = Format::WAV;
        wav.write_header(outputFile);
    }
    else if(path.ends_with(".mp3")) {
        format = Format::MP3;
    }
}

void AudioFile::flush() {
    outputFile.flush();
}

bool AudioFile::is_Open() {
    return outputFile.is_open();
}

void AudioFile::writeBatch(const Sample* batch) {
    if(format == Format::WAV) {
        wav.write_data(outputFile, batch);
         std::cout << "Batch Written";
         std::cout << "\n";
        samplesWritten += AudioConfig::SAMPLE_SIZE;
    } else if(format == Format::MP3) {
        throw std::runtime_error("MP3 saving not yet implemented");
    }
}

void AudioFile::finalWrite() {
    int dataSize = AudioFile::samplesWritten * sizeof(SampleSaved);
    wav.write_size(outputFile, dataSize);
    std::cout << "Final Write";
}

void AudioFile::close() {
    if(outputFile.is_open()) outputFile.close();
}
