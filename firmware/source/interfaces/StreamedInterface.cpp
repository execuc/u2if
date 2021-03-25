#include "StreamedInterface.h"
//#include <algorithm>

#include "tusb.h"

StreamedInterface::StreamedInterface(uint streamBufferSize)
    : _bufferRx(streamBufferSize), _totalRemainingBytesToSend(0){

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
    uint32_t nbByteCanRead = std::min(_bufferRx.getAllocateSize() - _bufferRx.size(), streamRxAvailableSize());
    _bufferRx.setSize(_bufferRx.size() + tud_cdc_read(_bufferRx.getDataPtr8() + _bufferRx.size(), nbByteCanRead));
    return _bufferRx.size();
}
