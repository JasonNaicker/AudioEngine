#pragma once

#include <thread>
#include <atomic>
#include "AudioTypes.h"  
#include <span>

class Producer {
public:
    Producer(AudioBuffer& audioBuffer, AudioBuffer& wavBuffer, std::span<const Sample> input, std::atomic<bool>& producerEnded);

    void start();
    void stop();

private:
    void worker();        

    std::atomic<bool>& producerEnded;
    AudioBuffer& audioBuffer;
    AudioBuffer& wavBuffer;
    std::atomic<bool> running{false};
    std::thread thread;
    std::span<const Sample> input;
};