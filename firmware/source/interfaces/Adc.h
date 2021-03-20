#ifndef _INTERFACE_ADC_H
#define _INTERFACE_ADC_H

#include "PicoInterfacesBoard.h"
#include "BaseInterface.h"


class Adc : public BaseInterface {
public:
    Adc();
    virtual ~Adc();

    CmdStatus process(uint8_t const *cmd, uint8_t response[64]);
    CmdStatus task(uint8_t response[64]);
protected:
    static int8_t getAdcIndexFromGpio(uint8_t gpio);
    CmdStatus gpioInit(uint8_t const *cmd);
    CmdStatus getValue(uint8_t const *cmd, uint8_t response[64]);
};


#endif

