#define MINIAUDIO_IMPLEMENTATION
#include "AudioStreamer.h"
#include "AudioFile.h"
#include "Wav.h"
#include <iostream>
#include <fstream>
#include <stdexcept>

AudioStreamer::AudioStreamer(std::span<const Sample> input, const std::string& outputPath) : audioBuffer(AudioConfig::BUFFER_SIZE), wavBuffer(AudioConfig::BUFFER_SIZE), producer(audioBuffer, input, producerEnded), consumer(wavBuffer, audioFile, playbackEnded) {
    /*
    if(!outputPath.empty()) {
        outputFile.open(outputPath, std::ios::binary);
        wav.write_header(outputFile);
    } */
    audioFile.open(outputPath);
    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format = ma_format_f32;
    config.playback.channels = AudioConfig::CHANNELS;
    config.sampleRate = AudioConfig::SAMPLE_RATE;
    config.periodSizeInFrames = AudioConfig::FRAME_SIZE;
    config.dataCallback = AudioStreamer::audioCallback;
    config.pUserData = this;

    if(ma_device_init(NULL, &config, &device) != MA_SUCCESS) {
        throw std::runtime_error("Failed to initialize audio device");
    }
};

void AudioStreamer::Start() {
    producer.start();
    if(audioFile.is_Open()) {
        consumer.start();
    }
    ma_device_start(&device);
}

void AudioStreamer::Stop() {
    producer.stop();
    ma_device_stop(&device);
    playbackEnded = true;
    if(audioFile.is_Open()) consumer.stop();
    ma_device_uninit(&device);
}

void AudioStreamer::Pause() {
    paused = true;
}

void AudioStreamer::Resume() {
    paused = false;
}

void AudioStreamer::audioCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    AudioStreamer* streamer = (AudioStreamer*) pDevice->pUserData;
    bool success = streamer->audioBuffer.read((Sample*) pOutput); //Read frame into pOutput
    if(success && streamer->audioFile.is_Open()) {
        //streamer->wav.write_data(streamer->outputFile, (Sample*) pOutput);
        streamer->wavBuffer.write((Sample*) pOutput); //Write frame into saving vbuffer
    }
}