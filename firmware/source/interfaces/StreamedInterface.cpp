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
    _bufferRx.setSize(tud_cdc_read(_bufferRx.getDataPtr(), std::min(_bufferRx.getAllocateSize(), streamRxAvailableSize())));
    return _bufferRx.size();
}
