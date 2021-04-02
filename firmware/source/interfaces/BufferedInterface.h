#ifndef _BUFFERED_INTERFACES_PICO_H
#define _BUFFERED_INTERFACES_PICO_H

#include <vector>
#include "BaseInterface.h"
#include "pico/critical_section.h"

struct Buffer {
    Buffer(uint32_t bufferSizeInBytes) : vector(bufferSizeInBytes/4 +1, 0){}

    enum BUFFER_STATE {
        FREE = 0x00,
        INPUT = 0x01,
        OUTPUT = 0x02
    };

    inline uint8_t* getDataPtr8() {return (uint8_t*)vector.data();}

    std::vector<uint32_t> vector; // to be 32-bit aligned for dma
    BUFFER_STATE state = FREE;
    uint32_t progressSize = 0;
    uint32_t size = 0;
};

class BufferedInterface : public BaseInterface {
public:
    BufferedInterface(uint32_t bufferSizeInBytes, uint32_t bufferCount);
    virtual ~BufferedInterface();

protected:
    Buffer* acquireInputBuffer();
    void releaseInputBuffer();
    Buffer* acquireOutputBuffer();
    void releaseOutputBuffer();
    void resetBuffers();
    //bool isOutputBufferAcquired();

    uint32_t _bufSize;
    uint32_t _bufCount;
    std::vector<Buffer> _circularBuffer;
    uint32_t _inputIndex, _outputIndex;
    critical_section_t _critSec;


    void flushStreamRx();
    uint32_t streamRxAvailableSize();
    bool streamRxRead();

};



#endif

