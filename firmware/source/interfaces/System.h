#ifndef _INTERFACE_SYSTEM_H
#define _INTERFACE_SYSTEM_H

#include "PicoInterfacesBoard.h"
#include "BaseInterface.h"


class System : public BaseInterface {
public:
    System();
    virtual ~System();

    CmdStatus process(uint8_t const *cmd, uint8_t response[64]);
    CmdStatus task(uint8_t response[64]);
protected:
    bool _needReset;
    uint count;
};


#endif

