#pragma once

#include "AudioTypes.h"
#include "AudioConfig.h"
#include "AudioMixer.h"

#include <fstream>
#include <vector>
#include <span>
#include <string>
#include <algorithm>
#include <cstdint>

#include <immintrin.h>
#include <xsimd/xsimd.hpp>

class Wav {
public:
    static constexpr int HEADER_SIZE = 36;

    template<typename T>
    void writeAsBytes(std::ofstream& file, T value, int byteSize) {
        for (int i = 0; i < byteSize; ++i) {
            char byte = static_cast<char>(value & 0xFF);
            file.write(&byte, 1);
            value >>= 8;
        }
    }
    
    void writeHeader(std::ofstream& file, const AudioFormatInfo& format) {
        constexpr int subchunk1Size = 16;
        constexpr int audioFormat = 1; 

        const int bitsPerSample = sizeof(SampleSaved) * 8;

        const int byteRate = format.sampleRate * format.channels * bitsPerSample / 8;

        const int blockAlign = format.channels * bitsPerSample / 8;

        // RIFF
        file.write("RIFF", 4);
        writeAsBytes(file, 0, 4);
        file.write("WAVE", 4);

        // fmt
        file.write("fmt ", 4);
        writeAsBytes(file, subchunk1Size, 4);
        writeAsBytes(file, audioFormat, 2);
        writeAsBytes(file, format.channels, 2);
        writeAsBytes(file, format.sampleRate, 4);
        writeAsBytes(file, byteRate, 4);
        writeAsBytes(file, blockAlign, 2);
        writeAsBytes(file, bitsPerSample, 2);

        // data
        file.write("data", 4);
        writeAsBytes(file, 0, 4); // patched later
    }

    // =====================================================
    // Offline save (arbitrary-sized buffer)
    // =====================================================

    void writeData(std::ofstream& file, std::span<const Sample> samples) {
        std::vector<SampleSaved> converted(samples.size());

        for (size_t i = 0; i < samples.size(); ++i) {
            float clamped = std::clamp(samples[i], -1.0f, 1.0f);
            converted[i] = static_cast<SampleSaved>(clamped * 32767.0f);
        }

        file.write(reinterpret_cast<const char*>(converted.data()), static_cast<std::streamsize>(converted.size() * sizeof(SampleSaved)));
    }

    // =====================================================
    // Realtime batch writer
    // =====================================================

    void writeBatch(std::ofstream& file, const Sample* batch) {
        alignas(32) int32_t temp[AudioConfig::SAMPLE_SIZE];
        SampleSaved converted[AudioConfig::SAMPLE_SIZE];

        for (size_t i = 0; i + SimdBatch::size <= AudioConfig::SAMPLE_SIZE; i += SimdBatch::size) {
            _mm_prefetch(reinterpret_cast<const char*>(&batch[i + 4 * SimdBatch::size]), _MM_HINT_T0);

            SimdBatch saveBatch = xsimd::load_unaligned(&batch[i]);
            saveBatch = AudioMixer::softClip(saveBatch);
            auto result = xsimd::to_int(saveBatch * 32767.0f);
            result.store_aligned(&temp[i]);
        }

        for (size_t i = 0; i < AudioConfig::SAMPLE_SIZE; ++i) {
            converted[i] = static_cast<SampleSaved>(temp[i]);
        }

        file.write(
            reinterpret_cast<const char*>(converted),
            AudioConfig::SAMPLE_SIZE *
            sizeof(SampleSaved)
        );
    }

    void writeSize(std::ofstream& file, size_t dataSize) {
        file.seekp(4);
        writeAsBytes(file, HEADER_SIZE + dataSize,4);
        file.seekp(40);
        writeAsBytes(file,dataSize, 4);
    }
};

/*#pragma once
#include "AudioConfig.h"
#include "AudioTypes.h"
#include "AudioMixer.h"
#include <iostream>
#include <fstream>
#include <cstdint>
#include <immintrin.h>
#include <xsimd/xsimd.hpp>

class Wav {
public:
    const int HEADER_SIZE = 36;

    const std::string CHUNK_ID = "RIFF";
    //CNUNK_SIZE
    const std::string FORMAT = "WAVE";

    const std::string SUBCHUNK1_ID = "fmt ";
    const int SUBCHUNK1_SIZE = 16;
    const int AUDIO_FORMAT = 1; //Compression amount, 1 = None
    const int NUM_CHANNELS = AudioConfig::CHANNELS; 
    const int SAMPLE_RATE = AudioConfig::SAMPLE_RATE;
    const int BITS_PER_SAMPLE = sizeof(SampleSaved) * 8;
    const int BYTE_RATE = SAMPLE_RATE * NUM_CHANNELS * BITS_PER_SAMPLE / 8;
    const int BLOCK_ALIGN = NUM_CHANNELS * BITS_PER_SAMPLE / 8;

    const std::string SUBCHUNK2_ID = "data";
    //SUBCHUNK2_SIZE
    template<typename T>
    void writeAsBytes(std::ofstream &file, T value, int byte_size) { //Little endian
        for(int i = 0; i < byte_size; ++i) {
            char byte = value & 0xFF; //Write in chunks of last 2 hex digits
            file.write(&byte, 1); //Write 1 byte of 2 digits
            value >>= 8; //Remove last 2 hex digits
        }
    }

    void writeHeader(std::ofstream &file) {
        //RIFF Chunk descriptor
        file.write(CHUNK_ID.c_str(), 4);
        writeAsBytes(file, 0, 4); //Write CHUNK_SIZE later
        file.write(FORMAT.c_str(), 4);

        //FMT subchunk
        file.write(SUBCHUNK1_ID.c_str(), 4);
        writeAsBytes(file, SUBCHUNK1_SIZE, 4);
        writeAsBytes(file, AUDIO_FORMAT, 2);
        writeAsBytes(file, NUM_CHANNELS, 2);
        writeAsBytes(file, SAMPLE_RATE, 4);
        writeAsBytes(file, BYTE_RATE, 4);
        writeAsBytes(file, BLOCK_ALIGN, 2);
        writeAsBytes(file, BITS_PER_SAMPLE, 2);

        //DATA subchunk
        file.write(SUBCHUNK2_ID.c_str(), 4);
        writeAsBytes(file, 0, 4); //Write SUBCHUNK2_SIZE later
        //Data at offset 44
    }

    //Add explicit vectorization loop
    void writeData(std::ofstream &file, const Sample* batch) {
        alignas(32) int32_t temp[AudioConfig::SAMPLE_SIZE];
        SampleSaved converted[AudioConfig::SAMPLE_SIZE];
        for(size_t i = 0; i + SimdBatch::size <= AudioConfig::SAMPLE_SIZE; i += SimdBatch::size) {
            _mm_prefetch((const char*) &batch[i + 4 * SimdBatch::size], _MM_HINT_T0);
            SimdBatch saveBatch = xsimd::load_unaligned(&batch[i]);
            saveBatch = AudioMixer::softClip(saveBatch);
            auto result = xsimd::to_int(saveBatch * 32767.0f);
            result.store_aligned(&temp[i]);
        }
        for(size_t i = 0; i < AudioConfig::SAMPLE_SIZE; i++) {
            converted[i] = static_cast<SampleSaved>(temp[i]);
        }
        file.write((const char*) converted, AudioConfig::SAMPLE_SIZE * sizeof(SampleSaved));

        /*
        for (size_t i = 0; i < AudioConfig::SAMPLE_SIZE; i++) {
            float clamped = std::clamp(batch[i], -1.0f, 1.0f);
            converted[i] = static_cast<SampleSaved>(clamped * 32767.0f);
        }
        file.write((const char*) converted, AudioConfig::SAMPLE_SIZE * sizeof(SampleSaved));
        */

       /*
    }

    void writeSize(std::ofstream &file, const size_t dataSize) {
        file.seekp(4);
        writeAsBytes(file, HEADER_SIZE + dataSize, 4);
        file.seekp(40);
        writeAsBytes(file, dataSize, 4);
        file.close();
    }
}; 
*/