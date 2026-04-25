#include "AudioClient.h"
#include <winsock2.h>
#include <stdexcept>

AudioClient::AudioClient() {
    WSADATA wsaData;

    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        throw std::runtime_error("WSAStartup Failed");
    }
}

AudioClient::~AudioClient() {
    WSACleanup();
}

void AudioClient::Start() {

}

void AudioClient::Stop() {

}