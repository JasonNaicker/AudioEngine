#pragma once

#include <thread>
#include <atomic>
#include "AudioTypes.h"  
#include <span>

class Consumer {
public:
    Consumer(AudioBuffer& buf, std::atomic<bool>& producerEnded);

    void start();
    void stop();

private:
    void worker();  

    std::atomic<bool>& producerEnded;
    AudioBuffer& buffer;
    std::atomic<bool> running{false};
    std::thread thread;
};