#pragma once

#include <thread>
#include <atomic>
#include "AudioTypes.h"  
#include <span>

class Producer {
public:
    Producer(AudioBuffer& buf, std::span<const Sample> input);

    void start();
    void stop();

private:
    void worker();        

    std::thread thread;
    std::atomic<bool> running{false};
    AudioBuffer& buffer;
    std::span<const Sample> input;
};