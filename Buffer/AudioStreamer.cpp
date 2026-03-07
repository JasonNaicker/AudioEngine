#include <iostream>
#include "Consumer.cpp"
#include "Producer.cpp"
#include "Wav.cpp"
#include "CircularBuffer.cpp"
#include <windows.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>
#include <guiddef.h>
#include <thread>

class AudioStreamer {
    public:
        AudioStreamer(CircularBuffer& buffer) : buffer(buffer) {}
    
    private:
        static const int BUFFER_SIZE = 8192;
        CircularBuffer& buffer;

};