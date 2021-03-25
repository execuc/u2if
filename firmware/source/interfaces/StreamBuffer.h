#ifndef _INTERFACE_STREAM_BUFFER_H
#define _INTERFACE_STREAM_BUFFER_H

#include <stdio.h>
#include <vector>


class StreamBuffer {
public:
    StreamBuffer(uint streamBufferSize);
    virtual ~StreamBuffer();
    inline uint32_t getAllocateSize() const {return static_cast<uint32_t>(_buffer.size()*4);}
    inline uint8_t* getDataPtr8() {return (uint8_t*)_buffer.data();}
    inline uint32_t* getDataPtr32() {return static_cast<uint32_t*>(_buffer.data());}
    inline uint32_t setSize(uint32_t size) {_bufSize = size;}
    inline uint32_t size() const {return _bufSize;}
protected:
    //std::vector<uint8_t> _buffer;
    std::vector<uint32_t> _buffer; // to be 32-bit aligned for ws2812b
    uint _bufSize;
};


#endif

