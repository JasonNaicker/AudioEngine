#pragma once
#include "miniaudio.h"
#include "AudioTypes.h"
#include "Consumer.h"
#include "Producer.h"
#include "AudioConfig.h"
#include <span>
#include <atomic>

class AudioStreamer {
public:
    AudioStreamer(std::span<const Sample> input);
    void Start();
    void Stop();

private:
    static void audioCallback(ma_device* device, void* output, const void* input, ma_uint32 frameCount);
    std::atomic<bool> producerEnded{false};
    AudioBuffer buffer;
    Producer producer;
    //Consumer consumer;
    ma_device device;
};