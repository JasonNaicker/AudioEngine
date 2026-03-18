#pragma once
#include <AudioConfig.h>
#include <AudioTypes.h>
#include <iostream>
#include <fstream>
#include <cstdint>

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
    const int BITS_PER_SAMPLE = sizeof(Sample) * 8;
    const int BYTE_RATE = SAMPLE_RATE * NUM_CHANNELS * BITS_PER_SAMPLE / 8;
    const int BLOCK_ALIGN = NUM_CHANNELS * BITS_PER_SAMPLE / 8;

    const std::string SUBCHUNK2_ID = "data";
    //SUBCHUNK2_SIZE
    template<typename T>
    void write_as_bytes(std::ofstream &file, T value, int byte_size) { //Little endian
        for(int i = 0; i < byte_size; ++i) {
            char byte = value & 0xFF; //Write in chunks of last 2 hex digits
            file.write(&byte, 1); //Write 1 byte of 2 digits
            value >>= 8; //Remove last 2 hex digits
        }
    }

    void write_header(std::ofstream &file) {
        //RIFF Chunk descriptor
        file.write(CHUNK_ID.c_str(), 4);
        write_as_bytes(file, 0, 4); //Write CHUNK_SIZE later
        file.write(FORMAT.c_str(), 4);

        //FMT subchunk
        file.write(SUBCHUNK1_ID.c_str(), 4);
        write_as_bytes(file, SUBCHUNK1_SIZE, 4);
        write_as_bytes(file, AUDIO_FORMAT, 2);
        write_as_bytes(file, NUM_CHANNELS, 2);
        write_as_bytes(file, SAMPLE_RATE, 4);
        write_as_bytes(file, BYTE_RATE, 4);
        write_as_bytes(file, BLOCK_ALIGN, 2);
        write_as_bytes(file, BITS_PER_SAMPLE, 2);

        //DATA subchunk
        file.write(SUBCHUNK2_ID.c_str(), 4);
        write_as_bytes(file, 0, 4); //Write SUBCHUNK2_SIZE later
        //Data at offset 44
    }

    void write_data(std::ofstream &file, const Sample* batch) {
        file.write((const char*) batch, AudioConfig::SAMPLE_SIZE * sizeof(Sample));
    }

    void write_size(std::ofstream &file, const size_t dataSize) {
        file.seekp(4);
        write_as_bytes(file, HEADER_SIZE + dataSize, 4);
        file.seekp(40);
        write_as_bytes(file, dataSize, 4);
        file.close();
    }
};