#pragma once
#include "AudioTypes.h"
#include <opus\opus.h>
#include <memory>
#include <vector>
class AudioDecoder {
public:
    AudioDecoder();
    std::vector<Sample> Decode(const std::vector<unsigned char>& batch);
    
private:
    std::unique_ptr<OpusDecoder, decltype(&opus_decoder_destroy)> decoder;
    int error = 0;
};