#pragma once
#include "AudioTypes.h"  
#include <thread>
#include <atomic>
#include <span>

class Producer {
public:
    Producer(AudioBuffer& audioBuffer, std::span<const Sample> input, std::atomic<bool>& producerEnded, std::atomic<bool>& paused);

    void start();
    void stop();

private:
    void worker();        

    std::atomic<bool>& producerEnded; //Unused
    AudioBuffer& audioBuffer;
    std::atomic<bool> running{false};
    std::atomic<bool>& paused;
    std::thread thread;
    std::span<const Sample> input;
    size_t sampleIndex;
};