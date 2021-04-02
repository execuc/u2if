#include "BufferedInterface.h"
//#include <algorithm>

#include "tusb.h"

BufferedInterface::BufferedInterface(uint32_t bufferSizeInBytes, uint32_t bufferCount)
    : _bufSize(bufferSizeInBytes), _bufCount(bufferCount),
      _circularBuffer(bufferCount, Buffer(bufferSizeInBytes + 1)),
      _inputIndex(0), _outputIndex(0) {
    critical_section_init(&_critSec);
}

BufferedInterface::~BufferedInterface() {

}


// TODO LOCK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Buffer* BufferedInterface::acquireInputBuffer() {
    /*printf("IO state: ");
    for(int i = 0; i<_bufCount; i++) {
        printf("[%d] ", _circularBuffer[i].state);
    }
    printf("\n");*/
    Buffer* pBuffer = nullptr;

    critical_section_enter_blocking(&_critSec);
    if(_circularBuffer[_inputIndex].state == Buffer::FREE) {
        pBuffer = &_circularBuffer[_inputIndex];
    }
    critical_section_exit(&_critSec);

    return pBuffer;
}

void BufferedInterface::releaseInputBuffer() {
    critical_section_enter_blocking(&_critSec);

    if(_circularBuffer[_inputIndex].state == Buffer::FREE) {
        _circularBuffer[_inputIndex].state = Buffer::INPUT;
        _inputIndex = (_inputIndex +1) %_bufCount;
    } else {
     // error ?
    }

    critical_section_exit(&_critSec);
    return;
}

Buffer* BufferedInterface::acquireOutputBuffer() {
    Buffer* pBuffer = nullptr;

    critical_section_enter_blocking(&_critSec);
    if(_circularBuffer[_outputIndex].state == Buffer::INPUT) {
        _circularBuffer[_outputIndex].state = Buffer::OUTPUT;
        pBuffer = &_circularBuffer[_outputIndex];
    }
    critical_section_exit(&_critSec);

    return pBuffer;
}

void BufferedInterface::releaseOutputBuffer() {
    critical_section_enter_blocking(&_critSec);

    if(_circularBuffer[_outputIndex].state == Buffer::OUTPUT) {
        _circularBuffer[_outputIndex].state = Buffer::FREE;
        _outputIndex = (_outputIndex +1) %_bufCount;
    } else {
        //error
    }
    critical_section_exit(&_critSec);
    return;
}

void BufferedInterface::resetBuffers() {
    critical_section_enter_blocking(&_critSec);
    for(uint i = 0; i<_bufCount; i++) {
        _circularBuffer[i].state = Buffer::FREE;
    }
    _outputIndex = 0;
    _inputIndex = 0;
    critical_section_exit(&_critSec);
}

/*
bool BufferedInterface::isOutputBufferAcquired() {
    bool res;
    critical_section_enter_blocking(&_critSec);
        res = (_circularBuffer[_outputIndex].state == Buffer::OUTPUT)
    critical_section_exit(&_critSec);
    return res;
}
*/


void BufferedInterface::flushStreamRx() {
    tud_cdc_read_flush();
}

uint32_t BufferedInterface::streamRxAvailableSize() {
    return tud_cdc_available();
}

bool BufferedInterface::streamRxRead() {
    Buffer &currentInputBuffer = _circularBuffer[_inputIndex];
    if(currentInputBuffer.state != Buffer::FREE)
        return 0; // error

    uint32_t nbByteCanRead = std::min(currentInputBuffer.size - currentInputBuffer.progressSize, streamRxAvailableSize());
    currentInputBuffer.progressSize += tud_cdc_read(currentInputBuffer.getDataPtr8() + currentInputBuffer.progressSize, nbByteCanRead);
    //printf("progressSize %d, size: %d", currentInputBuffer.progressSize, currentInputBuffer.size);
    return (currentInputBuffer.progressSize >= currentInputBuffer.size);
}
