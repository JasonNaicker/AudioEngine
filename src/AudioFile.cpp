#include "AudioFile.h"
#include "AudioConfig.h"
#include "AudioTypes.h"
#include "Wav.h"
#include <miniaudio.h>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

/* @deprecated
std::vector<Sample> AudioFile::reformat(const std::string& path, std::vector<Sample>& buffer) {
    AudioFormatInfo info = getFormat(path);

    ma_decoder_config config = ma_decoder_config_init(
        ma_format_f32,
        AudioConfig::CHANNELS,
        AudioConfig::SAMPLE_RATE
    );
        ma_data_converter_config config = ma_data_converter_config_init(
        info.format,
        ma_format_f32, 
        info.channels,
        AudioConfig::CHANNELS,
        info.sampleRate,
        AudioConfig::SAMPLE_RATE
    );

    ma_data_converter converter;
    if(ma_data_converter_init(&config, nullptr, &converter) != MA_SUCCESS) 
        throw std::runtime_error("Failed to init converter for: " + path);

    ma_uint64 frameCountIn = buffer.size() / info.channels; //Samples / channels = frameCount
    ma_uint64 frameCountOut = 0; //Updated frame count with new Frame rate
    ma_data_converter_get_expected_output_frame_count(&converter, frameCountIn, &frameCountOut); //Interpolate frames between sample rate
    
    std::vector<Sample> output(frameCountOut * AudioConfig::CHANNELS);
    if (ma_data_converter_process_pcm_frames(&converter, buffer.data(), &frameCountIn, output.data(), &frameCountOut)) 
        throw std::runtime_error("Failed to convert PCM frames for: " + path);

    ma_data_converter_uninit(&converter, nullptr);
    return output;
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
    throw std::runtime_error("MP3 is not implemented");
}

std::vector<Sample> AudioFile::load(const std::string& path) {
    std::vector<Sample> inputBuffer;
    if(path.ends_with(".pcm")) {
        inputBuffer = loadPCM(path);
    }
    else if(path.ends_with(".wav")) 
    {
        inputBuffer = loadWAV(path);
        inputBuffer = reformat(path, inputBuffer);
    }
    else if(path.ends_with(".mp3")) 
    {
        inputBuffer = loadMP3(path);
        inputBuffer = reformat(path, inputBuffer);
    }
    else throw std::runtime_error("Unsupported format " + path);
    pad(inputBuffer);
    return inputBuffer;
} */



std::vector<Sample> AudioFile::loadPCM(const std::string& path) {
    if(path.empty()) throw std::runtime_error("Path is empty");

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) throw std::runtime_error("Failed to load: " + path);

    size_t fileSize = std::filesystem::file_size(path);
    std::vector<Sample> inputBuffer(fileSize / sizeof(Sample));
    file.read((char*) inputBuffer.data(), fileSize);
    return inputBuffer;
}

AudioFormatInfo AudioFile::getFormat(const std::string& path) {
    if(path.empty()) throw std::runtime_error("Path is empty");
    ma_decoder decoder;
    if(ma_decoder_init_file(path.c_str(), nullptr, &decoder) != MA_SUCCESS) throw std::runtime_error("Failed to inspect: " + path);

    AudioFormatInfo info;
    ma_decoder_get_data_format(&decoder, &info.format, &info.channels, &info.sampleRate, nullptr, 0);

    ma_decoder_uninit(&decoder);
    return info;
}

std::vector<Sample> AudioFile::load(const std::string& path) {
    if (path.empty()) throw std::runtime_error("Path is empty");

    std::vector<Sample> buffer;
    if (path.ends_with(".pcm")) {
        buffer = loadPCM(path);
    } else {
        ma_decoder_config config = ma_decoder_config_init(
            ma_format_f32,
            AudioConfig::CHANNELS,
            AudioConfig::SAMPLE_RATE
        );

        ma_decoder decoder;

        if(ma_decoder_init_file(path.c_str(), &config, &decoder) != MA_SUCCESS)
            throw std::runtime_error("Failed to load: " + path);

        ma_uint64 totalFrames;
        ma_decoder_get_length_in_pcm_frames(&decoder, &totalFrames);

        buffer.resize(totalFrames * AudioConfig::CHANNELS);
        ma_decoder_read_pcm_frames(&decoder, buffer.data(), totalFrames, nullptr);
        ma_decoder_uninit(&decoder);
    }
    
    pad(buffer);
    return buffer;
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
        samplesWritten += AudioConfig::SAMPLE_SIZE;
    } else if(format == Format::MP3) {
        throw std::runtime_error("MP3 saving not yet implemented");
    }
}

void AudioFile::finalWrite() {
    int dataSize = AudioFile::samplesWritten * sizeof(SampleSaved);
    wav.write_size(outputFile, dataSize);
}

void AudioFile::close() {
    if(outputFile.is_open()) outputFile.close();
}
