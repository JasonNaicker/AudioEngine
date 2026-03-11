#include <iostream>
#include <thread>

class Producer {
    public:
        bool IS_RUNNING = false;
        void worker(int16_t* batch);
};

void Producer::worker(int16_t* batch) {
    while(IS_RUNNING) {

    }
}