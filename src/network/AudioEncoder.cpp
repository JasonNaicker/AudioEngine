#include "AudioEncoder.h"
#include "AudioConfig.h"
#include "AudioTypes.h"
#include <opus\opus.h>
#include <vector>

AudioEncoder::AudioEncoder() : encoder(nullptr, opus_encoder_destroy) {
    int error = 0;
    encoder.reset(opus_encoder_create(AudioConfig::SAMPLE_RATE, AudioConfig::CHANNELS, OPUS_APPLICATION_VOIP, &error));
    if (error != OPUS_OK) throw std::runtime_error("Could not construct Opus Encoder");
}

bool AudioEncoder::resetState() {
    int _valid = opus_encoder_ctl(encoder.get(), OPUS_RESET_STATE) == OPUS_OK;
    return _valid;
}

int AudioEncoder::getLookAhead() const {
    opus_int32 skip {};
    int _valid = opus_encoder_ctl(encoder.get(), OPUS_GET_LOOKAHEAD(&skip)) == OPUS_OK;
    return skip;
}

int AudioEncoder::getBandwidth() const {
    opus_int32 bw{};
    int _valid = opus_encoder_ctl(encoder.get(), OPUS_GET_BANDWIDTH(&bw)) == OPUS_OK;
    return _valid;
}

bool AudioEncoder::setBitrate(int bitrate) {
    int _valid = opus_encoder_ctl(encoder.get(), OPUS_SET_BITRATE(bitrate)) == OPUS_OK;
    return _valid;
}

bool AudioEncoder::setVariableBitrate(bool enabled) {
    int _valid = opus_encoder_ctl(encoder.get(), OPUS_SET_VBR(enabled)) == OPUS_OK;
    return _valid;
}

bool AudioEncoder::setComplexity(int complexity) {
    int _valid = opus_encoder_ctl(encoder.get(), OPUS_SET_COMPLEXITY(complexity)) == OPUS_OK;
    return _valid;
}

bool AudioEncoder::setInBandFEC(bool enabled) {
    int _valid = opus_encoder_ctl(encoder.get(), OPUS_SET_INBAND_FEC(enabled)) == OPUS_OK;
    return _valid;
}

bool AudioEncoder::setPacketLossPerc(int percent) {
    int _valid = opus_encoder_ctl(encoder.get(), OPUS_SET_PACKET_LOSS_PERC(percent)) == OPUS_OK;
    return _valid;
}

bool AudioEncoder::setDtx(bool enabled) {
    int _valid = opus_encoder_ctl(encoder.get(), OPUS_SET_DTX(enabled)) == OPUS_OK;
    return _valid;
}  

bool AudioEncoder::setBandwidth(int bandwidth)  {
    int _valid = opus_encoder_ctl(encoder.get(), OPUS_SET_BANDWIDTH(bandwidth)) == OPUS_OK;
    return _valid;
}

bool AudioEncoder::setLSBDepth(int depth) {
    int _valid = opus_encoder_ctl(encoder.get(), OPUS_SET_LSB_DEPTH(depth)) == OPUS_OK;
    return _valid;
}

int AudioEncoder::encode(std::span<const Sample> pcm, std::span<unsigned char> out) {
    const int frameSize = static_cast<int>(pcm.size() / AudioConfig::CHANNELS);
    const int bytes = opus_encode_float(
        encoder.get(), pcm.data(), frameSize,
        out.data(), static_cast<opus_int32>(out.size()));
    if (bytes < 0) throw std::runtime_error("Opus encode failed");
    return bytes;
}