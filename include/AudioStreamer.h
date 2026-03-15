#pragma once

#include "AudioTypes.h"
#include "Consumer.h"
#include "Producer.h"
#include "AudioConfig.h"
#include <span>

class AudioStreamer {
public:
    AudioStreamer(std::span<const Sample> input);
    void Start();
    void Stop();

private:
    AudioBuffer buffer;
    Consumer consumer;
    Producer producer;
    //std::span<const Sample> input;
};