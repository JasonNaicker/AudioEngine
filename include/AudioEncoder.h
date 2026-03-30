#pragma once
#include "AudioTypes.h"
#include <opus\opus.h>
#include <memory>
#include <vector>
class AudioEncoder {
public:
    AudioEncoder();
    std::vector<unsigned char> Encode(Sample* batch);

private:
    std::unique_ptr<OpusEncoder, decltype(&opus_encoder_destroy)> encoder;
    int error = 0;
};