#include "AudioStreamer.h"

AudioStreamer::AudioStreamer()
    : buffer(AudioConfig::BUFFER_SIZE),
      producer(buffer),
      consumer(buffer)
{
}

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