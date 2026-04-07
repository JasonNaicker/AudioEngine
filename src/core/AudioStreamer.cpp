#define MINIAUDIO_IMPLEMENTATION
#include "AudioStreamer.h"
#include "AudioFile.h"
#include "AudioTypes.h"
#include "MixConfig.h"
#include "Wav.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <immintrin.h>
#include <xsimd/xsimd.hpp>

AudioStreamer::AudioStreamer(const std::span<const Sample> input, bool useMic, const std::string& outputPath) : audioBuffer(AudioConfig::BUFFER_SIZE), saveBuffer(AudioConfig::BUFFER_SIZE), inputBuffer(AudioConfig::BUFFER_SIZE), producer(audioBuffer, input, producerEnded), consumer(saveBuffer, audioFile, playbackEnded), useMic(useMic){
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
    if(audioFile.isOpen()) {
        consumer.start();
    }
    ma_device_start(&device);
}

void AudioStreamer::Stop() {
    producer.stop();
    ma_device_stop(&device);
    playbackEnded = true;
    if(audioFile.isOpen()) consumer.stop();
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
    const Sample* input = (const Sample*) pInput;
    Sample* output = (Sample*) pOutput;

    alignas(32) Sample playbackAudio[AudioConfig::SAMPLE_SIZE] = {}; //Regular playback
    bool audioSuccess = streamer->audioBuffer.read((Sample*) playbackAudio); //Read into intermediary buffer

    for(size_t i = 0; i + 7 < AudioConfig::SAMPLE_SIZE; i += 8) {
        xsimd::batch<float> playbackBatch = xsimd::load_aligned(&playbackAudio[i]);
        xsimd::batch<float> playbackGain(MixConfig::MASTER_GAIN);
        playbackBatch *= playbackGain;

        xsimd::batch<float> micBatch = input ? xsimd::load_unaligned(&input[i]) : xsimd::batch<float>(0.0f);
        xsimd::batch<float> micGain(MixConfig::MIC_GAIN);
        micBatch *= micGain;

        xsimd::batch<float> result = xsimd::tanh(playbackBatch + micBatch);

        result.store_unaligned(&output[i]);
    }
    /* 
    for(size_t i = 0; i < AudioConfig::SAMPLE_SIZE; i++) {
        float playbackSample = playbackAudio[i] * MixConfig::MASTER_GAIN;
        float micSample = input ? input[i] * MixConfig::MIC_GAIN : 0.0f;

        output[i] = std::tanh(playbackSample + micSample);
    }
    */

    if(streamer->audioFile.isOpen()) {
        streamer->saveBuffer.write(output);
    }
}