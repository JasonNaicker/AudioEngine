#pragma once
#include "AudioTypes.h" 
#include "Wav.h"
#include <thread>
#include <atomic>
#include <fstream>
#include <span>
class Consumer {
public:
    Consumer(AudioBuffer& wavBuffer, std::atomic<bool>& producerEnded, std::ofstream& outputFile, Wav& wav);

    void start();
    void stop();

private:
    void worker(); 
    std::atomic<bool>& producerEnded; 
    std::ofstream& outputFile;
    Wav& wav;
    AudioBuffer& wavBuffer;
    std::atomic<bool> running{false};
    std::thread thread;
};