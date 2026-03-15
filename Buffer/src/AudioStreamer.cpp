#include "AudioStreamer.h"

AudioStreamer::AudioStreamer(std::span<const Sample> input) : buffer(AudioConfig::BUFFER_SIZE), producer(buffer),consumer(buffer) {}

void AudioStreamer::Start()
{
    producer.start();
    consumer.start();
}

void AudioStreamer::Stop()
{
    producer.stop();
    consumer.stop();
}