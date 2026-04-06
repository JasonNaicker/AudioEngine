#pragma once
#include "AudioTypes.h"  
#include <thread>
#include <atomic>
#include <span>

class Producer {
public:
    Producer(AudioBuffer& audioBuffer, std::span<const Sample> input, std::atomic<bool>& producerEnded);

    void start();
    void stop();

private:
    void worker();        

    std::atomic<bool>& producerEnded;
    AudioBuffer& audioBuffer;
    std::atomic<bool> running{false};
    std::thread thread;
    std::span<const Sample> input;
};