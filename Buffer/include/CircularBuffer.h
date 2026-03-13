#pragma once
#include <atomic>
#include <memory>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <stdexcept>

template<typename SampleType, size_t SAMPLE_SIZE>
class CircularBuffer {
public:
    CircularBuffer(std::size_t capacity);

    bool read(SampleType* batch) noexcept;
    bool write(const SampleType* batch) noexcept;

    static constexpr size_t SAMPLE_BYTES = SAMPLE_SIZE * sizeof(SampleType);

private:
    alignas(64) std::atomic<size_t> readPointer{0};
    alignas(64) std::atomic<size_t> writePointer{0};

    std::size_t mask;
    std::size_t capacity;
    std::unique_ptr<SampleType[]> buffer;
};

template<typename SampleType, size_t SAMPLE_SIZE>
CircularBuffer<SampleType, SAMPLE_SIZE>::CircularBuffer(std::size_t capacity)
    : capacity(capacity), readPointer(0), writePointer(0),
      buffer(std::make_unique<SampleType[]>(capacity)), mask(capacity - 1)
{
    if ((capacity & (capacity - 1)) != 0)
        throw std::invalid_argument("Capacity must be a power of 2");
    if (capacity <= SAMPLE_SIZE)
        throw std::invalid_argument("Capacity must be larger than sample size");
}

template<typename SampleType, size_t SAMPLE_SIZE>
bool CircularBuffer<SampleType, SAMPLE_SIZE>::read(SampleType* batch) noexcept {
    size_t read = readPointer.load(std::memory_order_relaxed);
    size_t write = writePointer.load(std::memory_order_acquire);

    size_t available = write - read; // Capped by write
    if (available < SAMPLE_SIZE) return false; // Check for underun

    size_t index = read & mask; // Index in buffer
    size_t spaceToEnd = capacity - index; // Space to the end

    size_t firstPart = std::min(SAMPLE_SIZE, spaceToEnd);
    std::memcpy(batch, buffer.get() + index, firstPart * sizeof(SampleType));
    std::memcpy(batch + firstPart, buffer.get(), (SAMPLE_SIZE - firstPart) * sizeof(SampleType));

    size_t nextRead = (read + SAMPLE_SIZE);
    readPointer.store(nextRead, std::memory_order_release);
    return true;
}

template<typename SampleType, size_t SAMPLE_SIZE>
bool CircularBuffer<SampleType, SAMPLE_SIZE>::write(const SampleType* batch) noexcept {
    size_t write = writePointer.load(std::memory_order_relaxed);
    // size_t read = readPointer.load(std::memory_order_acquire);

    size_t index = write & mask; // Index in buffer
    size_t spaceToEnd = capacity - index; // Space to the end

    size_t firstPart = std::min(SAMPLE_SIZE, spaceToEnd);
    std::memcpy(buffer.get() + index, batch, firstPart * sizeof(SampleType));
    std::memcpy(buffer.get(), batch + firstPart, (SAMPLE_SIZE - firstPart) * sizeof(SampleType));

    size_t nextWrite = (write + SAMPLE_SIZE);
    writePointer.store(nextWrite, std::memory_order_release);
    return true;
}