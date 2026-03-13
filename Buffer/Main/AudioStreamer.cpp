#include <iostream>
//#include <SDL3/SDL.h>

#include "CircularBuffer.h"

class AudioStreamer {
public:
    AudioStreamer(CircularBuffer& buffer) : buffer(buffer) {}

private:
    static const int BUFFER_SIZE = 8192;
    CircularBuffer& buffer;
};

int main() {

}