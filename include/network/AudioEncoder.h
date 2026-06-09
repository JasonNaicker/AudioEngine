#pragma once
#include "AudioTypes.h"
#include <opus\opus.h>
#include <memory>
#include <vector>
#include <span>
class AudioEncoder {
public: 
    static constexpr opus_int32 MAX_PACKET_SIZE = 4000;
    AudioEncoder();
    int encode(std::span<const Sample> pcm, std::span<unsigned char> out);
    bool resetState();
    bool setBitrate(int bitrate);
    bool setVariableBitrate(bool enabled);
    bool setComplexity(int complexity);
    bool setInBandFEC(bool enabled);
    bool setPacketLossPerc(int percent);   // 0–100
    bool setDtx(bool enabled);             // discontinuous transmission / comfort noise
    bool setBandwidth(int bandwidth);      // OPUS_BANDWIDTH_*
    bool setLSBDepth(int depth);           // 8–24

    int getBandwidth() const;
    int getLookAhead() const;

private:
    std::unique_ptr<OpusEncoder, decltype(&opus_encoder_destroy)> encoder;
};