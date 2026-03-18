#define MINIAUDIO_IMPLEMENTATION
#include "AudioStreamer.h"
#include <iostream>
#include <stdexcept>

AudioStreamer::AudioStreamer(std::span<const Sample> input) : buffer(AudioConfig::BUFFER_SIZE), producer(buffer, input, producerEnded) {//, consumer(buffer, producerEnded) {
    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format = ma_format_s16;
    config.playback.channels = AudioConfig::CHANNELS;
    config.sampleRate = AudioConfig::SAMPLE_RATE;
    config.periodSizeInFrames = AudioConfig::FRAME_SIZE;
    config.dataCallback = AudioStreamer::audioCallback;
    config.pUserData = this;

    if(ma_device_init(NULL, &config, &device) != MA_SUCCESS) {
        throw std::runtime_error("Failed to initialize audio device");
    }
}

void AudioStreamer::Start() {
    producer.start();
    //consumer.start();
    ma_device_start(&device);
}

void AudioStreamer::Stop() {
    producer.stop();
    //consumer.stop();
    ma_device_stop(&device);
    ma_device_uninit(&device);
}

void AudioStreamer::audioCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    AudioStreamer* streamer = (AudioStreamer*) pDevice->pUserData;
    bool success = streamer->buffer.read((Sample*) pOutput);
}