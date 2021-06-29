#include "StreamedInterface.h"
//#include <algorithm>

#include "tusb.h"

StreamedInterface::StreamedInterface(uint streamBufferSize, bool doubleBuffer)
    : _bufferRx(streamBufferSize), _bufferRx2(doubleBuffer ? streamBufferSize : 0), _totalRemainingBytesToSend(0), _currentBufferIndex(0){
}

StreamedInterface::~StreamedInterface() {

}

void StreamedInterface::flushStreamRx() {
    tud_cdc_read_flush();
}

uint32_t StreamedInterface::streamRxAvailableSize() {
    return tud_cdc_available();
}

uint32_t StreamedInterface::streamRxRead() {
    StreamBuffer &buf = getBuffer();
    uint32_t nbByteCanRead = std::min(buf.getAllocateSize() - buf.size(), streamRxAvailableSize());
    buf.setSize(buf.size() + tud_cdc_read(buf.getDataPtr8() + buf.size(), nbByteCanRead));
    return buf.size();
}
