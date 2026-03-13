#include <iostream>
#include <thread>
#include <atomic>

class Producer {
public:
    void start();
    void stop();

private:
    std::thread thread;
    std::atomic<bool> running{false};

    void worker();
};