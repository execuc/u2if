#ifndef _INTERFACE_MULTI_GPIO_H
#define _INTERFACE_MULTI_GPIO_H

#include "PicoInterfacesBoard.h"
#include "BaseInterface.h"
#include "pico/sync.h"

class GroupGpio : public BaseInterface {
public:
    GroupGpio();
    virtual ~GroupGpio();

    CmdStatus process(uint8_t const *cmd, uint8_t response[64]);
    CmdStatus task(uint8_t response[64]);

protected:
    //CmdStatus initMasksPins(uint8_t const *cmd);
    CmdStatus setMaskPins(uint8_t const *cmd);
    CmdStatus getAllPins(uint8_t const *cmd, uint8_t response[64]);
};


#endif

