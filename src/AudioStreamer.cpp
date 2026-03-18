#define MINIAUDIO_IMPLEMENTATION
#include "AudioStreamer.h"
#include "Wav.h"
#include <iostream>
#include <fstream>
#include <stdexcept>

AudioStreamer::AudioStreamer(std::span<const Sample> input, const std::string& outputPath) : audioBuffer(AudioConfig::BUFFER_SIZE), wavBuffer(AudioConfig::BUFFER_SIZE), producer(audioBuffer, wavBuffer, input, producerEnded), consumer(wavBuffer, producerEnded, outputFile, wav) {
    if(!outputPath.empty()) {
        outputFile.open(outputPath, std::ios::binary);
        wav.write_header(outputFile);
    }
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
    std::cout << "Starting producer\n";
    producer.start();
    
    std::cout << "File open: " << outputFile.is_open() << "\n";
    if(outputFile.is_open()) {
        std::cout << "Starting consumer\n";
        consumer.start();
    }
    
    std::cout << "Starting device\n";
    ma_device_start(&device);
    std::cout << "All started\n";
}

void AudioStreamer::Stop() {
    producer.stop();
    if(outputFile.is_open()) consumer.stop();
    ma_device_stop(&device);
    ma_device_uninit(&device);
}

void AudioStreamer::audioCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    AudioStreamer* streamer = (AudioStreamer*) pDevice->pUserData;
    bool success = streamer->audioBuffer.read((Sample*) pOutput);
    if(success && streamer->outputFile.is_open()) {
        streamer->wav.write_data(streamer->outputFile, (Sample*) pOutput);
    }
}