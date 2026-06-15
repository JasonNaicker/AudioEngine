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

std::vector<Sample> AudioFile::reformat(std::span<const uint8_t> buffer, const AudioFormatInfo& input, const AudioFormatInfo& output) {
    if (buffer.empty()) return {};

    const ma_uint32 inBytesPerFrame = ma_get_bytes_per_frame(input.format, input.channels);
    if (inBytesPerFrame == 0 || buffer.size() % inBytesPerFrame != 0)
        throw std::runtime_error("PCM data is not frame aligned for its declared format");

    const ma_uint64 inputFrames = buffer.size() / inBytesPerFrame;

    const ma_uint64 outputFrames = (inputFrames * output.sampleRate + input.sampleRate - 1) / input.sampleRate + 64;
    std::vector<Sample> outBuffer(outputFrames * output.channels);

    const ma_uint64 written = ma_convert_frames(outBuffer.data(), outputFrames, output.format, output.channels, output.sampleRate,
        buffer.data(), inputFrames, input.format, input.channels, input.sampleRate);

    outBuffer.resize(written * output.channels);
    return outBuffer;
}

std::vector<uint8_t> AudioFile::loadPCM(const std::string& path) {
    /*
    if(path.empty()) throw std::runtime_error("Path is empty");

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) throw std::runtime_error("Failed to load: " + path);

    size_t fileSize = std::filesystem::file_size(path);
    if (fileSize % sizeof(Sample) != 0) 
        throw std::runtime_error("PCM file must divide evenly into Sample Size: " + path);

    std::vector<Sample> inputBuffer(fileSize / sizeof(Sample));
    const std::streamsize bytes = static_cast<std::streamsize>(inputBuffer.size() * sizeof(Sample));
    file.read(reinterpret_cast<char*>(inputBuffer.data()), bytes);

    if (file.gcount() != bytes)
        throw std::runtime_error("Failed to load the full PCM file: " + path);
    return inputBuffer;
    */
    if(path.empty()) throw std::runtime_error("Path is empty");

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) throw std::runtime_error("Failed to load: " + path);

    const std::streamsize size = static_cast<std::streamsize>(std::filesystem::file_size(path));
    std::vector<uint8_t> bytes(static_cast<size_t>(size));
    file.read(reinterpret_cast<char*>(bytes.data()), size);

    if (file.gcount() != size)
        throw std::runtime_error("Failed to load the full PCM file: " + path);
    return bytes;
}

AudioFormatInfo AudioFile::getFormat(const std::string& path) {
    if(path.empty()) throw std::runtime_error("Path is empty");
    DecoderGuard dg;
    if(ma_decoder_init_file(path.c_str(), nullptr, &dg.dec) != MA_SUCCESS) throw std::runtime_error("Failed to inspect: " + path);

    AudioFormatInfo info;
    ma_decoder_get_data_format(&dg.dec, &info.format, &info.channels, &info.sampleRate, nullptr, 0);

    return info;
}

AudioData AudioFile::load(const std::string& path, std::optional<AudioFormatInfo> inputInfo) {
    if (path.empty()) throw std::runtime_error("Path is empty");

    AudioData data;
    std::vector<Sample> buffer;

    const AudioFormatInfo outputInfo = {
        ma_format_f32,
        AudioConfig::CHANNELS,
        AudioConfig::SAMPLE_RATE
    };

    if (path.ends_with(".pcm")) {
        if (!inputInfo.has_value()) 
            throw std::runtime_error("PCM file requries AudioFormatInfo: " + path);
        buffer = reformat(loadPCM(path), *inputInfo, outputInfo);
        data.sourceFormat = *inputInfo;
    } else {
        data.sourceFormat = getFormat(path); //get AudioFormatInfo

        //Always convert internally to this format
        ma_decoder_config config = ma_decoder_config_init(
            ma_format_f32,
            AudioConfig::CHANNELS,
            AudioConfig::SAMPLE_RATE
        );

        DecoderGuard dg;
        if (ma_decoder_init_file(path.c_str(), &config, &dg.dec) != MA_SUCCESS)
            throw std::runtime_error("Failed to load: " + path);
        dg.active = true;

        ma_uint64 totalFrames = 0;
        if (ma_decoder_get_length_in_pcm_frames(&dg.dec, &totalFrames) != MA_SUCCESS)
            throw std::runtime_error("Failed to get length in pcm frames: " + path);

        //Total ideal input sample size
        buffer.resize(totalFrames * AudioConfig::CHANNELS);

        ma_uint64 framesRead = 0;
        if (totalFrames > 0)
           ma_decoder_read_pcm_frames(&dg.dec, buffer.data(), totalFrames, &framesRead);

        //How many frames that were actually encoded
        buffer.resize(framesRead * AudioConfig::CHANNELS);
    }
    
    pad(buffer);
    data.buffer = std::move(buffer);
    data.runtimeFormat = outputInfo;
    return data;
}

void AudioFile::pad(std::vector<Sample>& buffer) {
    size_t remainder = buffer.size() % AudioConfig::SAMPLE_SIZE;
    if (remainder != 0) {
        buffer.resize(buffer.size() + (AudioConfig::SAMPLE_SIZE - remainder), 0);
    }
}

void AudioFile::save(const std::string& path, std::span<const Sample> buffer, const AudioFormatInfo& inputInfo) {
    std::ofstream saveFile(path, std::ios::binary);

    if (!saveFile)
        throw std::runtime_error("Failed to open save file: " + path);

    if (buffer.empty())
        throw std::runtime_error("Buffer cannot be empty for: " + path);

    if (inputInfo.channels == 0)
        throw std::runtime_error("Invalid channel count for: " + path);

    if (inputInfo.sampleRate == 0)
        throw std::runtime_error("Invalid sample rate for: " + path);

    if (buffer.size() % inputInfo.channels != 0)
        throw std::runtime_error("Buffer is not frame aligned for: " + path);

    if (path.ends_with(".pcm")) {
        const auto bytes = static_cast<std::streamsize>(buffer.size() * sizeof(Sample));
        saveFile.write(reinterpret_cast<const char*>(buffer.data()), bytes);

        if (!saveFile)
            throw std::runtime_error("Failed to write PCM data");
    }
    else if (path.ends_with(".wav")) {
        Wav wav;
        wav.writeHeader(saveFile, inputInfo);
        wav.writeData(saveFile, buffer);
        wav.writeSize(saveFile, buffer.size() * sizeof(SampleSaved));

    } else {
        throw std::runtime_error("Unsupported output format: " + path);
    }
}

//Open a file to write to
void AudioFile::openOutputFile(const std::string& path) {
    if(path.empty()) return;
    outputFile.open(path, std::ios::binary);

    if (!outputFile.is_open()) throw std::runtime_error("Failed to write to: " + path);

    const AudioFormatInfo runtimeInfo{
        ma_format_f32,
        AudioConfig::CHANNELS,
        AudioConfig::SAMPLE_RATE
    };

    if(path.ends_with(".wav")) {
        format = Format::WAV;
        wav.writeHeader(outputFile, runtimeInfo);
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
        wav.writeData(outputFile, std::span<const Sample> {batch, AudioConfig::SAMPLE_SIZE});
        samplesWritten += AudioConfig::SAMPLE_SIZE;
    } else if(format == Format::MP3) {
        throw std::runtime_error("MP3 saving not yet implemented");
    }
}

void AudioFile::finalWrite() {
    if(format == Format::WAV) {
        size_t dataSize = AudioFile::samplesWritten * sizeof(SampleSaved);
        wav.writeSize(outputFile, dataSize);
    } else if (format == Format::MP3) {
        throw std::runtime_error("MP3 saving not yet implemented");
    }
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