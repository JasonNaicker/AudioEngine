#include "Consumer.h"
#include "Wav.h"
#include <chrono>
#include <atomic>
#include <fstream>
#include <iostream>

Consumer::Consumer(AudioBuffer& buf, std::atomic<bool>& producerEnded, std::ofstream& outputFile, Wav& wav) : producerEnded(producerEnded), buffer(buf), outputFile(outputFile), wav(wav) {}

void Consumer::worker() {
    Sample data[AudioConfig::SAMPLE_SIZE];
    std::span<Sample> dataToRead(data, AudioConfig::SAMPLE_SIZE);
    size_t samplesWritten = 0;

    while (running) {
        bool success = buffer.read(dataToRead.data());
        if (success) {
            wav.write_data(outputFile, dataToRead.data());
            samplesWritten += AudioConfig::SAMPLE_SIZE;
            continue;
        } else {
            if(producerEnded) {
                int dataSize = samplesWritten * sizeof(Sample);
                wav.write_size(outputFile, dataSize);
                std::cout << "File saved..." << "\n";
                running = false;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
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