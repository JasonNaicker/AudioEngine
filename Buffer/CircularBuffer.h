#pragma once
#include <atomic>
#include <memory>
#include <cstdint>

class CircularBuffer {
public:
    CircularBuffer(std::size_t capacity);

    bool read(int16_t* batch) noexcept;
    bool write(const int16_t* batch) noexcept;

    static constexpr size_t CHANNELS = 2;
    static constexpr size_t FRAME_SIZE = 1024;
    static constexpr size_t SAMPLE_SIZE = FRAME_SIZE * CHANNELS;

private:
    alignas(64) std::atomic<size_t> readPointer;
    alignas(64) std::atomic<size_t> writePointer;

    std::size_t capacity;
    std::unique_ptr<int16_t[]> buffer;
};