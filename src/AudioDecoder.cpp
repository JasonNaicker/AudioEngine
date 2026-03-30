#include "AudioDecoder.h"
#include "AudioConfig.h"
#include "AudioTypes.h"
#include <opus\opus.h>
#include <vector>

AudioDecoder::AudioDecoder() : decoder(nullptr, opus_decoder_destroy), error(0) {

    decoder.reset(opus_decoder_create(AudioConfig::SAMPLE_RATE, AudioConfig::CHANNELS, &error)); 
}

std::vector<Sample> AudioDecoder::Decode(const std::vector<unsigned char>& batch) {
    std::vector<Sample> out(AudioConfig::SAMPLE_SIZE);
    int bytes = opus_decode_float(decoder.get(), batch.data(), batch.size(), out.data(), AudioConfig::FRAME_SIZE, 0);
    return out;
}