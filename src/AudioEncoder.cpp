#include "AudioEncoder.h"
#include "AudioConfig.h"
#include "AudioTypes.h"
#include <opus\opus.h>
#include <vector>

AudioEncoder::AudioEncoder() : encoder(nullptr, opus_encoder_destroy), error(0) {

    encoder.reset(opus_encoder_create(AudioConfig::SAMPLE_RATE, AudioConfig::CHANNELS, OPUS_APPLICATION_VOIP, &error));
}

std::vector<unsigned char> AudioEncoder::Encode(Sample* batch) {
    std::vector<unsigned char> out(4000);
    int bytes = opus_encode_float(encoder.get(), batch, AudioConfig::FRAME_SIZE, out.data(), 4000);
    if(bytes < 0) throw std::runtime_error("Opus encode failed");
    out.resize(bytes);
    return out;
}