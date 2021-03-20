#ifndef _INTERFACE_GPIO_H
#define _INTERFACE_GPIO_H

#include "PicoInterfacesBoard.h"
#include "BaseInterface.h"
#include "pico/sync.h"

class Gpio : public BaseInterface {
public:
    Gpio();
    virtual ~Gpio();

    CmdStatus process(uint8_t const *cmd, uint8_t response[64]);
    CmdStatus task(uint8_t response[64]);

protected:
    CmdStatus initPin(uint8_t const *cmd);
    CmdStatus setPin(uint8_t const *cmd);
    CmdStatus getPin(uint8_t const *cmd, uint8_t response[64]);
    CmdStatus setIrq(uint8_t const *cmd);
    CmdStatus getIrq(uint8_t const *cmd, uint8_t response[64]);
private:
    repeating_timer_t _debounceTimer;
};


#endif

