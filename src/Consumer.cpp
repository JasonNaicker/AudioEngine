#include "Consumer.h"
#include <chrono>
#include <atomic>
#include <iostream>

Consumer::Consumer(AudioBuffer& buf, std::atomic<bool>& producerEnded) : producerEnded(producerEnded), buffer(buf) {}

void Consumer::worker() {
    Sample data[AudioConfig::SAMPLE_SIZE];
    std::span<Sample> dataToRead(data, AudioConfig::SAMPLE_SIZE);

    while (running) {
        bool success = buffer.read(dataToRead.data());
        if (success) {
            continue;
        } else {
            if(producerEnded) {
                std::cout << "Consumer finished." << "\n";
                running = false;
                break;
            }
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