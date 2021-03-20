#ifndef _STREAMED_INTERFACES_PICO_H
#define _STREAMED_INTERFACES_PICO_H

#include "BaseInterface.h"
#include "StreamBuffer.h"

class StreamedInterface : public BaseInterface {
public:
    StreamedInterface(uint streamBufferSize);
    virtual ~StreamedInterface();

protected:
    void flushStreamRx();
    uint32_t streamRxAvailableSize();
    uint32_t streamRxRead();

    StreamBuffer _bufferRx;
    uint32_t _totalRemainingBytesToSend;
};



#endif

