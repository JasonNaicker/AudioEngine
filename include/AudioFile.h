#pragma once

#include "miniaudio.h"
#include "AudioConfig.h"
#include "AudioTypes.h"
#include "Wav.h"
#include <vector>
#include <fstream>
#include <string>
#include <stdexcept>

struct AudioFormatInfo {
    ma_format format;
    ma_uint32 channels;
    ma_uint32 sampleRate;
};

class AudioFile {
public:
    enum class Format {WAV, MP3, NONE};

    static std::vector<Sample> load(const std::string& path);
    static void pad(std::vector<Sample>& buffer);

    void open(const std::string& path);
    void writeBatch(const Sample* batch);
    void finalWrite();
    void close();
    void flush();
    bool is_Open();   

private:
    static std::vector<Sample> loadPCM(const std::string& path);
    static std::vector<Sample> loadWAV(const std::string& path);
    static std::vector<Sample> loadMP3(const std::string& path);
    static AudioFormatInfo getFormat(const std::string& path);
    static std::vector<Sample> reformat(const std::string& path, std::vector<Sample>& buffer);
    std::ofstream outputFile;
    size_t samplesWritten{0}; 
    Wav wav;
    Format format{Format::NONE};
};