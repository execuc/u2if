#ifndef _STREAMED_INTERFACES_PICO_H
#define _STREAMED_INTERFACES_PICO_H

#include "BaseInterface.h"
#include "StreamBuffer.h"

class StreamedInterface : public BaseInterface {
public:
    StreamedInterface(uint streamBufferSize, bool doubleBuffer = false);
    virtual ~StreamedInterface();
    //inline void setCurrentBufferIndex(uint8_t index) {_currentBufferIndex = index;}
    inline uint32_t getCurrentBufferIndex() const { return _currentBufferIndex;}
    inline void switchBuffer() { _currentBufferIndex = (_currentBufferIndex+1) %2;}
    inline StreamBuffer & getBuffer() {return (_currentBufferIndex == 0 ? _bufferRx : _bufferRx2);}

protected:
    void flushStreamRx();
    uint32_t streamRxAvailableSize();
    uint32_t streamRxRead();

    StreamBuffer _bufferRx;
    StreamBuffer _bufferRx2;
    uint32_t _totalRemainingBytesToSend;
    int _currentBufferIndex;
};



#endif

