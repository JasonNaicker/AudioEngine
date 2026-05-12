#include "AudioServer.h"
#include <winsock2.h>
#include <stdexcept>

AudioServer::AudioServer() {

}

AudioServer::~AudioServer() {
    WSACleanup();
}

void AudioServer::Start() {

}

void AudioServer::Stop() {

}