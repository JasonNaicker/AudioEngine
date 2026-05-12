#include "AudioClient.h"
#include <winsock2.h>
#include <stdexcept>

AudioClient::AudioClient() {


}

AudioClient::~AudioClient() {
    WSACleanup();
}

void AudioClient::Start() {

}

void AudioClient::Stop() {

}