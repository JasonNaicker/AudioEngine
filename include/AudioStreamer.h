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

private:
    static void audioCallback(ma_device* device, void* output, const void* input, ma_uint32 frameCount);
    std::ofstream outputFile;
    Wav wav;
    std::atomic<bool> producerEnded{false};
    AudioBuffer audioBuffer;
    AudioBuffer wavBuffer;
    Producer producer;
    Consumer consumer;
    ma_device device;

};