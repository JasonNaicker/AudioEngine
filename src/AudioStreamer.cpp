#include "AudioStreamer.h"
#include <iostream>

AudioStreamer::AudioStreamer(std::span<const Sample> input)
    : buffer(AudioConfig::BUFFER_SIZE), producer(buffer, input, producerEnded), consumer(buffer, producerEnded) {}

void AudioStreamer::Start() {
    producer.start();
    consumer.start();
}

void AudioStreamer::Stop() {
    producer.stop();
    consumer.stop();
}