#include <iostream>
#include <atomic>
#include <cstring>
#include <thread>
#include <chrono>
#include <cmath>

class CircularBuffer {
    public:
        CircularBuffer(std::size_t capacity) : capacity(capacity), readPointer(0), writePointer(0) {
            buffer = new int16_t[capacity]; 
        }

        ~CircularBuffer() {
            delete[] buffer;
        }

        inline bool read(int16_t* batch) noexcept {
            size_t read = readPointer.load(std::memory_order_relaxed);
            size_t write = writePointer.load(std::memory_order_acquire);

            size_t available = (capacity + (write - read)) & (capacity - 1);

            if (available < SAMPLE_SIZE) return false; //Check for underun

            size_t spaceToEnd = capacity - read;

            if (SAMPLE_SIZE <= spaceToEnd) {
                memcpy(batch, buffer + read, SAMPLE_SIZE * sizeof(int16_t));
            } else {
                memcpy(batch, buffer + read, spaceToEnd * sizeof(int16_t));
                memcpy(batch + spaceToEnd, buffer, (SAMPLE_SIZE - spaceToEnd) * sizeof(int16_t));
            }

            size_t nextRead = (read + SAMPLE_SIZE) & (capacity - 1);
            readPointer.store(nextRead, std::memory_order_release);
            return true;
        }

        inline bool write(const int16_t* batch) noexcept {
            size_t write = writePointer.load(std::memory_order_relaxed);
            size_t read = readPointer.load(std::memory_order_acquire);
            
            size_t spaceToEnd = capacity - write;

            if (SAMPLE_SIZE <= spaceToEnd) {
                std::memcpy(buffer + write, batch, SAMPLE_SIZE * sizeof(int16_t));
            } else {
                std::memcpy(buffer + write, batch, spaceToEnd * sizeof(int16_t));
                std::memcpy(buffer, batch + spaceToEnd, (SAMPLE_SIZE - spaceToEnd) * sizeof(int16_t));
            }

            size_t nextWrite = (write + SAMPLE_SIZE) & (capacity - 1);
            writePointer.store(nextWrite, std::memory_order_release);
            return true;
        }
        
    private:
        alignas(64) std::atomic<size_t> readPointer;
        alignas(64) std::atomic<size_t> writePointer;
        std::size_t capacity;
        int16_t* buffer;
    
    public:
        static constexpr size_t CHANNELS = 2;
        static constexpr size_t FRAME_SIZE = 1024; //Number of Frames
        static constexpr size_t SAMPLE_SIZE = FRAME_SIZE * CHANNELS; 
};