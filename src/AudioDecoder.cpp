#include "AudioDecoder.h"
#include "AudioConfig.h"
#include "AudioTypes.h"

AudioDecoder::AudioDecoder() : decoder(nullptr, opus_decoder_destroy), error(error) {

    decoder.reset(opus_encoder_create(AudioConfig::SAMPLE_RATE, AudioConfig::CHANNELS, OPUS_APPLICATION_VOIP, &error));

}