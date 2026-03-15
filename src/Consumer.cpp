#include "Consumer.h"
#include <chrono>
#include <iostream>

Consumer::Consumer(AudioBuffer& buf) : buffer(buf) {}

void Consumer::worker() {
    size_t sampleIndex = 0;

    Sample data[AudioConfig::SAMPLE_SIZE];
    std::span<Sample> dataToRead(data, AudioConfig::SAMPLE_SIZE);
    while (running) {
        bool success = buffer.read(dataToRead.data());

        if(success) {
            std::cout << "Consumer read a batch of " << AudioConfig::SAMPLE_SIZE << " samples\n";
            sampleIndex += AudioConfig::SAMPLE_SIZE;
        } else {
            std::cout << "Consumer is waiting (buffer empty)\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

void Consumer::start() {
    running = true;
    thread = std::thread(&Consumer::worker, this);
}

void Consumer::stop() {
    running = false;
    if (thread.joinable())
        thread.join();
}