#include "AudioEncoder.h"
#include "AudioConfig.h"
#include "AudioTypes.h"

AudioEncoder::AudioEncoder() : encoder(nullptr, opus_encoder_destroy), error(error) {

    encoder.reset(opus_encoder_create(AudioConfig::SAMPLE_RATE, AudioConfig::CHANNELS, OPUS_APPLICATION_VOIP, &error));
}

void AudioEncoder::Encode(Sample* batch) {
    unsigned char out[AudioConfig::FRAME_SIZE];
    opus_encode_float(&encoder, batch, AudioConfig::FRAME_SIZE, out, 4000);
}