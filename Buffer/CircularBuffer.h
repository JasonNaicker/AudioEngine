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
    static constexpr size_t SAMPLE_BYTES = SAMPLE_SIZE * sizeof(int16_t);

private:
    alignas(64) std::atomic<size_t> readPointer{0};
    alignas(64) std::atomic<size_t> writePointer{0};

    std::size_t mask;
    std::size_t capacity;
    std::unique_ptr<int16_t[]> buffer;
};