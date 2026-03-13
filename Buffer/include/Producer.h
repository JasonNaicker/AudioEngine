#pragma once

#include <thread>
#include <atomic>
#include "AudioTypes.h"  

class Producer {
public:
    Producer(AudioBuffer& buf);

    void start();
    void stop();

private:
    void worker();        

    std::thread thread;
    std::atomic<bool> running{false};
    Producer& buffer;
};