#include "Producer.h"
#include "AudioConfig.h"
#include <chrono>
#include <iostream>

Producer::Producer(AudioBuffer& buf, std::span<const Sample> input) : buffer(buf), input(input) {};

void Producer::worker() {
    const size_t inputSize = input.size();
    size_t sampleIndex = 0;
    while (running) {
        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if(sampleIndex + AudioConfig::SAMPLE_SIZE <= inputSize) { //For testing, change for partial batches
            std::span<const Sample> dataToWrite = input.subspan(sampleIndex, AudioConfig::SAMPLE_SIZE);
            bool success = buffer.write(dataToWrite.data());

            if(success) {
                std::cout << "Producer wrote a batch of " << AudioConfig::SAMPLE_SIZE << " samples\n";
                sampleIndex += AudioConfig::SAMPLE_SIZE;
            } else {
                std::cout << "Producer is waiting (buffer full)\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }

        } else {
            std::cout << "Producer finished processing input.\n";
            Producer::stop();
        }
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