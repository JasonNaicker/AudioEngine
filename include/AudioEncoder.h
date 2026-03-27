#pragma once
#include "AudioTypes.h"
#include <opus\opus.h>
#include <memory>

class AudioEncoder {
public:
    AudioEncoder();
    void Encode(Sample* batch);

private:
    std::unique_ptr<OpusEncoder, decltype(&opus_encoder_destroy)> encoder;
    int error = 0;
};