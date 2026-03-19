#pragma once
#include "miniaudio.h"
#include "AudioTypes.h"
#include "Consumer.h"
#include "Producer.h"
#include "AudioConfig.h"
#include "Wav.h"
#include <fstream>
#include <string>
#include <span>
#include <atomic>

class AudioStreamer {
public:
    AudioStreamer(std::span<const Sample> input, const std::string& outputPath = "");
    void Start();
    void Stop();
    void Pause();
    void Resume();

private:
    static void audioCallback(ma_device* device, void* output, const void* input, ma_uint32 frameCount);
    AudioFile audioFile;
    std::atomic<bool> paused{false};
    std::atomic<bool> producerEnded{false};
    std::atomic<bool> playbackEnded{false};
    AudioBuffer audioBuffer;
    AudioBuffer wavBuffer;
    Producer producer;
    Consumer consumer;
    ma_device device;

};