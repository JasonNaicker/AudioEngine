#include <iostream>
#include <thread>
#include <atomic>

class Consumer {
public:
    void start();
    void stop();

private:
    std::thread thread;
    std::atomic<bool> running{false};

    void worker();
};