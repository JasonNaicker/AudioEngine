#pragma once

#include "AudioTypes.h"
#include "Consumer.h"
#include "Producer.h"
#include "AudioConfig.h"

class AudioStreamer {
public:
    AudioStreamer();

    void Start();
    void Stop();

private:
    AudioBuffer buffer;
    Consumer consumer;
    Producer producer;
};