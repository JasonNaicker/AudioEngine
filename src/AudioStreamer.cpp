#include "AudioStreamer.h"
#include <iostream>
#include <mutex>

std::mutex coutMutex;

AudioStreamer::AudioStreamer(std::span<const Sample> input)
    : buffer(AudioConfig::BUFFER_SIZE), producer(buffer, input), consumer(buffer) {}

void AudioStreamer::Start() {
    producer.start();
    consumer.start();
}

void AudioStreamer::Stop() {
    producer.stop();
    consumer.stop();
}