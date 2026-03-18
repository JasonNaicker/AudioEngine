#pragma once
#include "AudioTypes.h" 
#include "Wav.h"
#include <thread>
#include <atomic>
#include <fstream>
#include <span>
class Consumer {
public:
    Consumer(AudioBuffer& buf, std::atomic<bool>& producerEnded, std::ofstream& outputFile, Wav& wav);

    void start();
    void stop();

private:
    void worker();  
    std::ofstream& outputFile;
    Wav& wav;
    std::atomic<bool>& producerEnded;
    AudioBuffer& buffer;
    std::atomic<bool> running{false};
    std::thread thread;
};