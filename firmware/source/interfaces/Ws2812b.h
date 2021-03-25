#ifndef _INTERFACE_WS2812B_H
#define _INTERFACE_WS2812B_H

#include "PicoInterfacesBoard.h"
#include "StreamedInterface.h"
#include "pico/sync.h"

class Ws2812b : public StreamedInterface {
public:
    Ws2812b(uint maxLeds);
    virtual ~Ws2812b();

    CmdStatus process(uint8_t const *cmd, uint8_t response[64]);
    CmdStatus task(uint8_t response[64]);

protected:
    enum INTERNAL_STATE {
        IDLE = 0x00,
        WAIT_PIXELS = 0x01,
        //WAIT_START_TRANSFER = 0x02,
        TRANSFER_IN_PROGRESS = 0x03,
        TRANSFER_FINISHED = 0x04
    };

    CmdStatus init(uint8_t const *cmd);
    CmdStatus deinit(uint8_t const *cmd);
    CmdStatus write(uint8_t const *cmd, uint8_t response[64]);
    void startTransfer(uint32_t *pixelBuf, uint32_t nbPixels);

    uint32_t _maxLeds;
    INTERNAL_STATE _internalState;
};


#endif

