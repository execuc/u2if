#include "BaseInterface.h"

#include <algorithm>
#include "tusb.h"


BaseInterface::BaseInterface() :
    _interfaceState(InterfaceState::NOT_INITIALIZED) {
}

BaseInterface::~BaseInterface(){

}


CmdStatus BaseInterface::process(uint8_t const *cmd, uint8_t response[64]) {
    (void)cmd;
    (void)response;
    return CmdStatus::NOT_CONCERNED;
}

CmdStatus BaseInterface::task(uint8_t response[64]) {
    (void)response;
    return CmdStatus::NOT_CONCERNED;
}

void BaseInterface::convertUInt32ToBytes(uint32_t value, uint8_t *array) {
    array[0] = static_cast<uint8_t>(value & 0xFF);
    array[1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    array[2] = static_cast<uint8_t>((value >> 16) & 0xFF);
    array[3] = static_cast<uint8_t>((value >> 24) & 0xFF);
}

void BaseInterface::convertUInt16ToBytes(uint16_t value, uint8_t *array) {
    array[0] = static_cast<uint8_t>(value & 0xFF);
    array[1] = static_cast<uint8_t>((value >> 8) & 0xFF);
}

uint32_t BaseInterface::convertBytesToUInt32(const uint8_t *array) {
    const uint32_t value = static_cast<uint32_t>(array[0]) | (static_cast<uint32_t>(array[1]) << 8u) | (static_cast<uint32_t>(array[2]) << 16u) | (static_cast<uint32_t>(array[3]) << 24u);
    return value;
}

uint16_t BaseInterface::convertBytesToUInt16(const uint8_t *array) {
    const uint16_t value = static_cast<uint16_t>(array[0]) | (static_cast<uint16_t>(array[1]) << 8u);
    return value;
}
