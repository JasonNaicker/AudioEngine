#include "CircularBuffer.h"
#include <iostream>
#include <atomic>
#include <cstring>

    CircularBuffer::CircularBuffer(std::size_t capacity) : capacity(capacity), readPointer(0), writePointer(0), buffer(std::make_unique<int16_t[]>(capacity)) {
        if ((capacity & (capacity - 1)) != 0) throw std::invalid_argument("Capacity must be a power of 2");
        if (capacity <= SAMPLE_SIZE) throw std::invalid_argument("Capacity must be larger than sample size");
    }   

    bool CircularBuffer::read(int16_t* batch) noexcept {
        size_t read = readPointer.load(std::memory_order_relaxed);
        size_t write = writePointer.load(std::memory_order_acquire);

        size_t available = (capacity + (write - read)) & (capacity - 1);

        if (available < SAMPLE_SIZE) return false; //Check for underun

        size_t spaceToEnd = capacity - read;

        if (SAMPLE_SIZE <= spaceToEnd) {
            memcpy(batch, buffer.get() + read, SAMPLE_SIZE * sizeof(int16_t));
        } else {
            memcpy(batch, buffer.get() + read, spaceToEnd * sizeof(int16_t));
            memcpy(batch + spaceToEnd, buffer.get(), (SAMPLE_SIZE - spaceToEnd) * sizeof(int16_t));
        }

        size_t nextRead = (read + SAMPLE_SIZE) & (capacity - 1);
        readPointer.store(nextRead, std::memory_order_release);
        return true;
    }

    bool CircularBuffer::write(const int16_t* batch) noexcept {
        size_t write = writePointer.load(std::memory_order_relaxed);
        size_t read = readPointer.load(std::memory_order_acquire);
        
        size_t spaceToEnd = capacity - write;

        if (SAMPLE_SIZE <= spaceToEnd) {
            std::memcpy(buffer.get() + write, batch, SAMPLE_SIZE * sizeof(int16_t));
        } else {
            std::memcpy(buffer.get() + write, batch, spaceToEnd * sizeof(int16_t));
            std::memcpy(buffer.get(), batch + spaceToEnd, (SAMPLE_SIZE - spaceToEnd) * sizeof(int16_t));
        }

        size_t nextWrite = (write + SAMPLE_SIZE) & (capacity - 1);
        writePointer.store(nextWrite, std::memory_order_release);
        return true;
    }