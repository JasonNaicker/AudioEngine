#pragma once
#include "AudioTypes.h"
#include <opus\opus.h>
#include <memory>

class AudioDecoder {
public:
    AudioDecoder();
    void Decode(Sample* batch);
    
private:
    std::unique_ptr<OpusDecoder, decltype(&opus_decoder_destroy)> decoder;
    int error = 0;
};