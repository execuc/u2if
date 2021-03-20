#ifndef _BASE_INTERFACES_PICO_H
#define _BASE_INTERFACES_PICO_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "../tusb_config.h"

#include "PicoInterfacesBoard.h"

enum InterfaceState {
    NOT_INITIALIZED = 1,
    INTIALIZED = 2
};

class BaseInterface {
public:

    BaseInterface();
    virtual ~BaseInterface();
    virtual CmdStatus process(uint8_t const *cmd, uint8_t response[64]);
    virtual CmdStatus task(uint8_t response[64]);
    inline InterfaceState getInterfaceState() const { return _interfaceState;}
    static void convertUInt32ToBytes(uint32_t value, uint8_t *array);
    static void convertUInt16ToBytes(uint16_t value, uint8_t *array);
    static uint32_t convertBytesToUInt32(const uint8_t *array);
    static uint16_t convertBytesToUInt16(const uint8_t *array);

protected:
    inline void setInterfaceState(InterfaceState interfaceState) {_interfaceState = interfaceState;}
    

    InterfaceState _interfaceState;
};



#endif

