#include "AudioFile.h"
#include "AudioConfig.h"
#include "AudioTypes.h"
#include "Wav.h"
#include <miniaudio.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <span>
#include <optional>

std::vector<Sample> AudioFile::reformat(std::span<const Sample> buffer, const AudioFormatInfo& input, const AudioFormatInfo& output) {
    if (buffer.empty()) return {};

    if (input.format == output.format && input.channels == output.channels && input.sampleRate == output.sampleRate) 
        return std::vector<Sample>(buffer.begin(),buffer.end()); 

    if (buffer.size() % input.channels != 0) 
        throw std::runtime_error("Invalid buffer alignment");

    ma_data_converter_config config = ma_data_converter_config_init(
        input.format,
        output.format,
        input.channels,
        output.channels,
        input.sampleRate,
        output.sampleRate
    );

    ma_data_converter converter;
    if(ma_data_converter_init( &config, nullptr, &converter) != MA_SUCCESS) 
        throw std::runtime_error("Failed to init converter for: ");

    ma_uint64 frameCountIn = buffer.size() / input.channels; //Samples / channels = frameCount
    ma_uint64 frameCountOut = 0; //Updated frame count with new Frame rate
    //ceil(inputFrames * outputSampleRate / inputSampleRate)
    ma_data_converter_get_expected_output_frame_count(&converter, frameCountIn, &frameCountOut); //Interpolate frames between sample rate
    frameCountOut += 64;

    std::vector<Sample> outBuffer(frameCountOut * output.channels);
    if (ma_data_converter_process_pcm_frames(&converter, buffer.data(), &frameCountIn, outBuffer.data(), &frameCountOut) != MA_SUCCESS) {
        ma_data_converter_uninit(&converter, nullptr);
        throw std::runtime_error("Failed to convert PCM frames");
    }

    ma_data_converter_uninit(&converter, nullptr);
    outBuffer.resize(frameCountOut * output.channels);
    return outBuffer;
}

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

AudioData AudioFile::load(const std::string& path, std::optional<AudioFormatInfo> info) {
    if (path.empty()) throw std::runtime_error("Path is empty");

    AudioData data;
    std::vector<Sample> buffer;

    if (path.ends_with(".pcm")) {
        if (!info.has_value()) 
            throw std::runtime_error("PCM file requries AudioFormatInfo: " + path);
        buffer = loadPCM(path);
        data.sourceFormat = *info;
    } else {
        data.sourceFormat = getFormat(path); //get AudioFormatInfo

        ma_decoder_config config = ma_decoder_config_init(
            ma_format_f32,
            AudioConfig::CHANNELS,
            AudioConfig::SAMPLE_RATE
        );

        ma_decoder decoder;

        if(ma_decoder_init_file(path.c_str(), &config, &decoder) != MA_SUCCESS) {
            ma_decoder_uninit(&decoder);
            throw std::runtime_error("Failed to load: " + path);
        }

        ma_uint64 totalFrames;
        ma_decoder_get_length_in_pcm_frames(&decoder, &totalFrames);

        buffer.resize(totalFrames * AudioConfig::CHANNELS);
        ma_decoder_read_pcm_frames(&decoder, buffer.data(), totalFrames, nullptr);
        ma_decoder_uninit(&decoder);
    }
    
    pad(buffer);
    data.buffer = buffer;
    data.runtimeFormat = {
        ma_format_f32,
        AudioConfig::CHANNELS,
        AudioConfig::SAMPLE_RATE
    };
    return data;
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
        wav.writeHeader(outputFile);
    }
    else if(path.ends_with(".mp3")) {
        format = Format::MP3;
    }
}

void AudioFile::flush() {
    outputFile.flush();
}

bool AudioFile::isOpen() const {
    return outputFile.is_open();
}

void AudioFile::writeBatch(const Sample* batch) {
    if(format == Format::WAV) {
        wav.writeData(outputFile, batch);
        samplesWritten += AudioConfig::SAMPLE_SIZE;
    } else if(format == Format::MP3) {
        throw std::runtime_error("MP3 saving not yet implemented");
    }
}

void AudioFile::finalWrite() {
    int dataSize = AudioFile::samplesWritten * sizeof(SampleSaved);
    wav.writeSize(outputFile, dataSize);
}

void AudioFile::close() {
    if(outputFile.is_open()) outputFile.close();
}

/* @deprecated
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