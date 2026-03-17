#include "Producer.h"
#include "AudioConfig.h"
#include <chrono>
#include <atomic>
#include <iostream>

Producer::Producer(AudioBuffer& buf, std::span<const Sample> input, std::atomic<bool>& producerEnded) : producerEnded(producerEnded), buffer(buf), input(input){};

void Producer::worker() {
    const size_t inputSize = input.size();
    size_t sampleIndex = 0;
    while (running) {
        //Fixed input only, otherwise dynamic input never ends
        if(sampleIndex == inputSize) {
            std::cout << "Producer finished." << "\n";
            running = false;
            producerEnded = true;
            break;
        }
        std::span<const Sample> dataToWrite = input.subspan(sampleIndex, AudioConfig::SAMPLE_SIZE);
        bool success = buffer.write(dataToWrite.data());
        sampleIndex += AudioConfig::SAMPLE_SIZE;
    }
}

void Producer::start() {
    running = true;
    thread = std::thread(&Producer::worker, this);
}

void Producer::stop() {
    running = false;
    if (thread.joinable())
        thread.join();
}