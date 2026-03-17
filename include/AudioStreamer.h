#pragma once

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
    std::atomic<bool> producerEnded{false};
    AudioBuffer buffer;
    Producer producer;
    Consumer consumer;
    //std::span<const Sample> input;
};