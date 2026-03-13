#include "Producer.h"
#include <chrono>

Producer::Producer(AudioBuffer& buf) : buffer(buf) {}

void Producer::worker() {
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
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