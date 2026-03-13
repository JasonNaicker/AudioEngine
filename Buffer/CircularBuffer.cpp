#include "CircularBuffer.h"
#include <iostream>
#include <atomic>
#include <cstring>

    CircularBuffer::CircularBuffer(std::size_t capacity) : capacity(capacity), readPointer(0), writePointer(0), buffer(std::make_unique<int16_t[]>(capacity)), mask(capacity - 1) {
        if ((capacity & (capacity - 1)) != 0) throw std::invalid_argument("Capacity must be a power of 2");
        if (capacity <= SAMPLE_SIZE) throw std::invalid_argument("Capacity must be larger than sample size");
    }   

    bool CircularBuffer::read(int16_t* batch) noexcept {
        size_t read = readPointer.load(std::memory_order_relaxed);
        size_t write = writePointer.load(std::memory_order_acquire);

        size_t available = write - read; //Capped by write

        if (available < SAMPLE_SIZE) return false; //Check for underun

        size_t index = read & mask; //Index in buffer
        size_t spaceToEnd = capacity - index; //Space to the end

        size_t firstPart = std::min(SAMPLE_SIZE, spaceToEnd);
        memcpy(batch, buffer.get() + index, firstPart * sizeof(int16_t));
        memcpy(batch + firstPart, buffer.get(), (SAMPLE_SIZE - firstPart) * sizeof(int16_t));

        size_t nextRead = (read + SAMPLE_SIZE);
        readPointer.store(nextRead, std::memory_order_release);
        return true;
    }

    bool CircularBuffer::write(const int16_t* batch) noexcept {
        size_t write = writePointer.load(std::memory_order_relaxed);
        //size_t read = readPointer.load(std::memory_order_acquire);
        
        size_t index = write & mask; //Index in buffer
        size_t spaceToEnd = capacity - index; //Space to the end

        size_t firstPart = std::min(SAMPLE_SIZE, spaceToEnd);
        std::memcpy(buffer.get() + index, batch, firstPart * sizeof(int16_t));
        std::memcpy(buffer.get(), batch + firstPart, (SAMPLE_SIZE - firstPart) * sizeof(int16_t));

        size_t nextWrite = (write + SAMPLE_SIZE);
        writePointer.store(nextWrite, std::memory_order_release);
        return true;
    }