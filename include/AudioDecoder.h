#pragma once
#include <opus\opus.h>
#include <memory>

class AudioDecoder {
public:
    AudioDecoder();
    void Decode(Sample* batch);
private:
    std::unique_ptr<OpusEncoder, decltype(&opus_decoder_destroy)> decoder;
    int& error;
};