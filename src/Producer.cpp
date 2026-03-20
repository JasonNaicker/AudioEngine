#include "Producer.h"
#include "AudioConfig.h"
#include <chrono>
#include <atomic>
#include <iostream>

Producer::Producer(AudioBuffer& audioBuffer, std::span<const Sample> input, std::atomic<bool>& producerEnded) : producerEnded(producerEnded), audioBuffer(audioBuffer), input(input){};

void Producer::worker() {
    if (input.empty()) {
        running = false;
        producerEnded = true;
        return;
    }
    const size_t inputSize = input.size();
    size_t sampleIndex = 0;
    while (running) {
        //Fixed input only, otherwise dynamic input never ends
        if(sampleIndex == inputSize) {
            running = false;
            producerEnded = true;
            break;
        }
        std::span<const Sample> dataToWrite = input.subspan(sampleIndex, AudioConfig::SAMPLE_SIZE);
        bool success_play = audioBuffer.write(dataToWrite.data());

        if(success_play) {
            sampleIndex += AudioConfig::SAMPLE_SIZE;
        } else {
              std::this_thread::sleep_for(std::chrono::milliseconds(1));
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