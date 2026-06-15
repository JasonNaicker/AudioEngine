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
#include <cstddef>

struct DecoderGuard {
    ma_decoder dec{};
    bool active = false;
    ~DecoderGuard() {if (active) ma_decoder_uninit(&dec);}
};
struct ConverterGuard {
    ma_data_converter conv{};
    bool active = false;
    ~ConverterGuard() {if (active) ma_data_converter_uninit(&conv, nullptr);}
};

class AudioFile {
public:
    enum class Format {WAV, MP3, FLAC, NONE};
    
    static AudioData load(const std::string& path, std::optional<AudioFormatInfo> inputInfo = std::nullopt);
    static void pad(std::vector<Sample>& buffer);
    void openOutputFile(const std::string& path);
    void writeBatch(const Sample* batch);
    void finalWrite();
    void close();
    void flush();
    bool isOpen() const;
    static void save(const std::string& path, std::span<const Sample> buffer, const AudioFormatInfo& inputInfo);
    static std::vector<Sample> reformat(std::span<const uint8_t> buffer, const AudioFormatInfo& input, const AudioFormatInfo& output = {});
    static AudioFormatInfo getFormat(const std::string& path);

private:
    static std::vector<uint8_t> loadPCM(const std::string& path);
    //static std::vector<Sample> loadWAV(const std::string& path);
    //static std::vector<Sample> loadMP3(const std::string& path);
    //static std::vector<Sample> loadFlac(const std::string* path);
    std::ofstream outputFile;
    size_t samplesWritten{0}; 
    Wav wav;
    Format format{Format::NONE};
};