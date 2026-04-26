#pragma once
#include "miniaudio.h"
#include "AudioConfig.h"
#include "AudioTypes.h"
#include "Wav.h"
#include <vector>
#include <span>
#include <fstream>
#include <string>
#include <stdexcept>
#include <optional>

struct AudioFormatInfo {
    ma_format format = ma_format_f32;
    ma_uint32 channels = AudioConfig::CHANNELS;
    ma_uint32 sampleRate = AudioConfig::SAMPLE_RATE;
};

struct AudioData {
    std::vector<Sample> buffer;
    AudioFormatInfo sourceFormat;
    AudioFormatInfo runtimeFormat;
};
class AudioFile {
public:
    enum class Format {WAV, MP3, FLAC, NONE};

    static AudioData load(const std::string& path, std::optional<AudioFormatInfo> info = std::nullopt);
    static void pad(std::vector<Sample>& buffer);
    void open(const std::string& path);
    void writeBatch(const Sample* batch);
    void finalWrite();
    void close();
    void flush();
    bool isOpen() const;
    static void save(const std::string& path, std::span<const Sample>& buffer);
    static std::vector<Sample> reformat(std::span<const Sample> buffer, const AudioFormatInfo& input, const AudioFormatInfo& output = {});
    static AudioFormatInfo getFormat(const std::string& path);

private:
    static std::vector<Sample> loadPCM(const std::string& path);
    //static std::vector<Sample> loadWAV(const std::string& path);
    //static std::vector<Sample> loadMP3(const std::string& path);
    //static std::vector<Sample> loadFlac(const std::string* path);
    std::ofstream outputFile;
    size_t samplesWritten{0}; 
    Wav wav;
    Format format{Format::NONE};
};