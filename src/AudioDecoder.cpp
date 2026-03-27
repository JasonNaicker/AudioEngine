#include "AudioDecoder.h"
#include "AudioConfig.h"
#include "AudioTypes.h"

AudioDecoder::AudioDecoder() : decoder(nullptr, opus_decoder_destroy), error(error) {

    decoder.reset(opus_decoder_create(AudioConfig::SAMPLE_RATE, AudioConfig::CHANNELS, &error)); 

}