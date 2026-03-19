#pragma once
#include "AudioTypes.h" 
#include "AudioFile.h"
#include "Wav.h"
#include <thread>
#include <atomic>
#include <fstream>
#include <span>
class Consumer {
public:
    Consumer(AudioBuffer& wavBuffer, AudioFile& audioFile, std::atomic<bool>& playbackEnded);

    void start();
    void stop();

private:
    void worker(); 
    std::atomic<bool>& playbackEnded; 
    AudioFile& audioFile;
    AudioBuffer& wavBuffer;
    std::atomic<bool> running{false};
    std::thread thread;
};