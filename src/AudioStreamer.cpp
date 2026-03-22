#define MINIAUDIO_IMPLEMENTATION
#include "AudioStreamer.h"
#include "AudioFile.h"
#include "Wav.h"
#include <iostream>
#include <fstream>
#include <stdexcept>

AudioStreamer::AudioStreamer(std::span<const Sample> input, bool useMic, const std::string& outputPath) : audioBuffer(AudioConfig::BUFFER_SIZE), saveBuffer(AudioConfig::BUFFER_SIZE), inputBuffer(AudioConfig::BUFFER_SIZE), producer(audioBuffer, input, producerEnded), consumer(saveBuffer, audioFile, playbackEnded), useMic(useMic){
    /*
    if(!outputPath.empty()) {
        outputFile.open(outputPath, std::ios::binary);
        wav.write_header(outputFile);
    } */

    audioFile.open(outputPath);
    ma_device_type deviceType = useMic ? ma_device_type_duplex : ma_device_type_playback;
    ma_device_config config = ma_device_config_init(deviceType);
    config.playback.format = ma_format_f32;
    config.playback.channels = AudioConfig::CHANNELS;
    //config.noPreSilencedOutputBuffer = MA_FALSE;
    //config.noClip = MA_FALSE;
    if(useMic) {
        config.capture.format = ma_format_f32;
        config.capture.channels = AudioConfig::CHANNELS;
    }

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

    const Sample* mic = (const Sample*) pInput;
    Sample playbackAudio[AudioConfig::SAMPLE_SIZE] = {}; //Regular playback

    bool audioSuccess = streamer->audioBuffer.read((Sample*) playbackAudio); //Read into intermediary buffer

    Sample* out = (Sample*) pOutput; //Final output buffer

    for(size_t i = 0; i < AudioConfig::SAMPLE_SIZE; i++) {
        float playback = playbackAudio[i] * 1.0f;
        float micSample = mic ? mic[i] * 1.0f : 0.0f;

        out[i] = std::tanh(playback + micSample);
    }

    /*
    streamer->inputBuffer.write((Sample*) pInput); //Write mic input to input buffer
    bool success = streamer->audioBuffer.read((Sample*) pOutput); //Read buffer for playback
    if(success && streamer->audioFile.is_Open()) {
        streamer->saveBuffer.write((const Sample*) pOutput); //Write frame to saving buffer
    } */

    if(streamer->audioFile.is_Open()) {
        streamer->saveBuffer.write(out);
    }
}