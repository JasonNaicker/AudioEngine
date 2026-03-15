#include "Producer.h"
#include "AudioConfig.h"
#include <chrono>
#include <iostream>
#include <mutex>

extern std::mutex coutMutex;

Producer::Producer(AudioBuffer& buf, std::span<const Sample> input) : buffer(buf), input(input) {}

void Producer::worker() {
    const size_t inputSize = input.size();
    size_t sampleIndex = 0;

    size_t batchesWritten = 0;
    size_t stallCount = 0;

    auto startTime = std::chrono::high_resolution_clock::now();
    while (running) {
        if (sampleIndex + AudioConfig::SAMPLE_SIZE <= inputSize) {
            std::span<const Sample> dataToWrite = input.subspan(sampleIndex, AudioConfig::SAMPLE_SIZE);

            auto writeStart = std::chrono::high_resolution_clock::now();
            bool success = buffer.write(dataToWrite.data());

            if (success) {
                auto writeEnd = std::chrono::high_resolution_clock::now();
                double writeUs = std::chrono::duration<double, std::micro>(writeEnd - writeStart).count();
                
                batchesWritten++;
                sampleIndex += AudioConfig::SAMPLE_SIZE;

                std::lock_guard<std::mutex> lock(coutMutex);
                std::cout << "[Producer] Batch " << batchesWritten
                          << " written in " << writeUs << " us\n";
            } else {
                stallCount++;
                std::lock_guard<std::mutex> lock(coutMutex);
                std::cout << "[Producer] STALL #" << stallCount << " (buffer full)\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        } else {
            auto endTime = std::chrono::high_resolution_clock::now();
            double totalMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
            double throughput = (batchesWritten * AudioConfig::SAMPLE_SIZE) / (totalMs / 1000.0);

            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << "\n[Producer] Done.\n"
                      << "  Batches written : " << batchesWritten << "\n"
                      << "  Total stalls    : " << stallCount << "\n"
                      << "  Total time      : " << totalMs << " ms\n"
                      << "  Throughput      : " << throughput << " samples/sec\n";
            running = false;
        }
    }
}

void Producer::start() {
    running = true;
    thread = std::thread(&Producer::worker, this);
}

void Producer::stop() {
    running = false;
    if (thread.joinable())
        thread.join();
}