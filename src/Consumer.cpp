#include "Consumer.h"
#include <chrono>
#include <iostream>
#include <mutex>

extern std::mutex coutMutex;

Consumer::Consumer(AudioBuffer& buf) : buffer(buf) {}

void Consumer::worker() {
    Sample data[AudioConfig::SAMPLE_SIZE];
    std::span<Sample> dataToRead(data, AudioConfig::SAMPLE_SIZE);

    size_t batchesRead = 0;
    size_t underrunCount = 0;
    double totalLatencyUs = 0.0;
    double maxLatencyUs = 0.0;
    auto startTime = std::chrono::high_resolution_clock::now();

    while (running) {
        auto readStart = std::chrono::high_resolution_clock::now();
        bool success = buffer.read(dataToRead.data());
        if (success) {
            auto readEnd = std::chrono::high_resolution_clock::now();
            double latencyUs = std::chrono::duration<double, std::micro>(readEnd - readStart).count();
            
            batchesRead++;
            totalLatencyUs += latencyUs;
            if (latencyUs > maxLatencyUs) maxLatencyUs = latencyUs;

            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << "[Consumer] Batch " << batchesRead
                      << " read in " << latencyUs << " us\n";
        } else {
            underrunCount++;
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << "[Consumer] UNDERRUN #" << underrunCount << " (buffer empty)\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            running = false;
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    double totalMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    double avgLatencyUs = batchesRead > 0 ? totalLatencyUs / batchesRead : 0.0;
    double throughput = (batchesRead * AudioConfig::SAMPLE_SIZE) / (totalMs / 1000.0);

    std::lock_guard<std::mutex> lock(coutMutex);
    std::cout << "\n[Consumer] Done.\n"
              << "  Batches read    : " << batchesRead << "\n"
              << "  Underruns       : " << underrunCount << "\n"
              << "  Avg latency     : " << avgLatencyUs << " us\n"
              << "  Max latency     : " << maxLatencyUs << " us\n"
              << "  Total time      : " << totalMs << " ms\n"
              << "  Throughput      : " << throughput << " samples/sec\n";
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