#ifndef _INTERFACE_PWM_H
#define _INTERFACE_PWM_H

#include "PicoInterfacesBoard.h"
#include "BaseInterface.h"

struct Slice {
    int gpioChannelA = -1;
    int gpioChannelB = -1;
    bool isFreqComputed = false;

    inline bool isFree() {return (gpioChannelA == -1 && gpioChannelB == -1);}

    inline bool isGpioChannelUsed(uint channel) {
        if(channel == 0)
            return gpioChannelA >= 0;
        else
            return gpioChannelB >= 0;
    }

    inline bool setGpioChannel(uint channel, uint gpio) {
        if(channel == 0)
            return gpioChannelA = gpio;
        else
            return gpioChannelB = gpio;
    }

    inline bool unsetGpioChannel(uint channel) {
        if(channel == 0)
            gpioChannelA = -1;
        else
            gpioChannelB = -1;

        if (isFree()) {
            isFreqComputed = false;
        }
    }
};

class Pwm : public BaseInterface {
public:
    Pwm();
    virtual ~Pwm();

    CmdStatus process(uint8_t const *cmd, uint8_t response[64]);
    CmdStatus task(uint8_t response[64]);

protected:
    CmdStatus initPwm(uint8_t const *cmd, uint8_t response[64]);
    CmdStatus deInitPwm(uint8_t const *cmd);
    CmdStatus setFreq(uint8_t const *cmd, uint8_t response[64]);
    CmdStatus getFreq(uint8_t const *cmd, uint8_t response[64]);
    CmdStatus setDutyU16(uint8_t const *cmd);
    CmdStatus getDutyU16(uint8_t const *cmd, uint8_t response[64]);
    CmdStatus setDutyNs(uint8_t const *cmd, uint8_t response[64]);
    CmdStatus getDutyNs(uint8_t const *cmd, uint8_t response[64]);

    Slice _sliceArray[8];
};


#endif

