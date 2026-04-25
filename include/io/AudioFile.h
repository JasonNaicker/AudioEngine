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

struct AudioFormatInfo {
    ma_format format = ma_format_f32;
    ma_uint32 channels = AudioConfig::CHANNELS;
    ma_uint32 sampleRate = AudioConfig::SAMPLE_RATE;
};
class AudioFile {
public:
    enum class Format {WAV, MP3, FLAC, NONE};

    static std::vector<Sample> load(const std::string& path);
    static void pad(std::vector<Sample>& buffer);

    void open(const std::string& path);
    void writeBatch(const Sample* batch);
    void finalWrite();
    void close();
    void flush();
    bool isOpen() const;

private:
    static std::vector<Sample> loadPCM(const std::string& path);
    //static std::vector<Sample> loadWAV(const std::string& path);
    //static std::vector<Sample> loadMP3(const std::string& path);
    //static std::vector<Sample> loadFlac(const std::string* path);
    static AudioFormatInfo getFormat(const std::string& path);
    static std::vector<Sample> reformat(const std::string& path, std::span<const Sample> buffer, const AudioFormatInfo& opts = {});
    std::ofstream outputFile;
    size_t samplesWritten{0}; 
    Wav wav;
    Format format{Format::NONE};
};