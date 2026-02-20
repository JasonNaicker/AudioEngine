#include <iostream>
#include <fstream>
#include <cstdint>

class Wav {
    public:
        const std::string CHUNK_ID = "RIFF";
        const std::string FORMAT = "WAVE";

        const std::string SUBCHUNK1_ID = "fmt ";
        const int SUBCHUNK1_SIZE = 16;
        const int AUDIO_FORMAT = 1; //Compression amount, 1 = None
        const int NUM_CHANNELS = 2; //Stereo
        const int SAMPLE_RATE = 44100;
        const int BITS_PER_SAMPLE = 16;
        const int BYTE_RATE = SAMPLE_RATE * NUM_CHANNELS * BITS_PER_SAMPLE / 8;
        const int BLOCK_ALIGN = NUM_CHANNELS * BITS_PER_SAMPLE / 8;

        const std::string SUBCHUNK2_ID = "data";

        template<typename T>
        void write_as_bytes(std::ofstream &file, T value, int byte_size) {
            for(int i = 0; i < byte_size; ++i) {
                char byte = value & 0xFF;
                file.write(&byte, 1);
                value >>= 8;
            }
        }

        void write_header(std::ofstream &file, int SUBCHUNK2_SIZE) {
            //RIFF Chunk descriptor
            int chunk_size = 36 + SUBCHUNK2_SIZE;
            file.write(CHUNK_ID.c_str(), 4);
            write_as_bytes(file, chunk_size, 4);
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
            write_as_bytes(file, SUBCHUNK2_SIZE, 4);
        }
};

int main() {
    
    Wav wav;
    std::ofstream file("C:\\Users\\Jason\\Downloads\\Test.wav", std::ios::binary);
    int data_size = 0;
    wav.write_header(file, data_size);
    file.close();
    return 0;
}