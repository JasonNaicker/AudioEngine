#pragma once

#include <thread>
#include <atomic>
#include "AudioTypes.h"  
#include <span>

class Producer {
public:
    Producer(AudioBuffer& buf, std::span<const Sample> input, std::atomic<bool>& producerEnded);

    void start();
    void stop();

private:
    void worker();        

    std::atomic<bool>& producerEnded;
    AudioBuffer& buffer;
    std::atomic<bool> running{false};
    std::thread thread;
    std::span<const Sample> input;
};