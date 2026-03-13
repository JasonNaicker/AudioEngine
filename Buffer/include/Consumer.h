#pragma once

#include <thread>
#include <atomic>
#include "AudioTypes.h"  

class Consumer {
public:
    Consumer(AudioBuffer& buf);

    void start();
    void stop();

private:
    void worker();        

    std::thread thread;
    std::atomic<bool> running{false};
    AudioBuffer& buffer;
};