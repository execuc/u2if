#ifndef _INTERFACE_I2S_H
#define _INTERFACE_I2S_H

#include "PicoInterfacesBoard.h"
#include "BufferedInterface.h"
#include "pico/sync.h"

class I2s : public BufferedInterface {
public:
    I2s(uint32_t bufferSizeInBytes, uint32_t bufferCount);
    virtual ~I2s();

    CmdStatus process(uint8_t const *cmd, uint8_t response[64]);
    CmdStatus task(uint8_t response[64]);

    void handleDmaIrq();

protected:
    enum INPUT_STATE {
        IDLE = 0x00,
        WAIT_INPUT = 0x01,
    };



    CmdStatus init(uint8_t const *cmd);
    CmdStatus deinit(uint8_t const *cmd);
    CmdStatus setFreq(uint8_t const *cmd);
    CmdStatus writeBuffer(uint8_t const *cmd, uint8_t response[64]);
    void initDma();
    void startDma();
    bool update_pio_frequency(uint32_t sample_freq);
    uint _offsetProgram;

    INPUT_STATE _inputState;

};


#endif

