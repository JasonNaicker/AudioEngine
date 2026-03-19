#include "Consumer.h"
#include "Wav.h"
#include "AudioFile.h"
#include <chrono>
#include <atomic>
#include <fstream>
#include <iostream>

Consumer::Consumer(AudioBuffer& wavBuffer, AudioFile& audioFile, std::atomic<bool>& playbackEnded) : playbackEnded(playbackEnded), audioFile(audioFile), wavBuffer(wavBuffer) {};

void Consumer::worker() {
    std::cout << "Consumer started\n";
    Sample data[AudioConfig::SAMPLE_SIZE];
    std::span<Sample> dataToRead(data, AudioConfig::SAMPLE_SIZE);

    while (running) {
        bool success = wavBuffer.read(dataToRead.data());
        if (success) {
            audioFile.writeBatch(dataToRead.data());
            continue;
        } else {
            if(playbackEnded) {
                std::cout<<"END CONSUMER";
                while (wavBuffer.read(dataToRead.data())) {
                    audioFile.writeBatch(dataToRead.data());
                }
                audioFile.flush();
                audioFile.finalWrite();
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
    //running = false;
    if (thread.joinable())
        thread.join();
}