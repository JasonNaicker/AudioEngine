#include "Consumer.h"
#include <chrono>

Consumer::Consumer(AudioBuffer& buf) : buffer(buf) {}

void Consumer::worker() {
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
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