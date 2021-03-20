#ifndef _INTERFACE_SPI_MASTER_H
#define _INTERFACE_SPI_MASTER_H

#include "PicoInterfacesBoard.h"
#include "StreamedInterface.h"
#include "hardware/spi.h"


class SPIMaster : public StreamedInterface {
public:
    SPIMaster(uint8_t i2cIndex, uint streamBufferSize);
    virtual ~SPIMaster();

    CmdStatus process(uint8_t const *cmd, uint8_t response[64]);
    CmdStatus task(uint8_t response[64]);

protected:
    CmdStatus init(uint8_t const *cmd);
    CmdStatus deInit();
    CmdStatus write(const uint8_t *cmd);
    CmdStatus writeFromUart(const uint8_t *cmd);
    CmdStatus read(const uint8_t *cmd, uint8_t *ret);
    uint8_t getInstIndex();

    spi_inst_t *_spiInst;
    uint _clkGP;
    uint _mosiGP;
    uint _misoGP;
};

#endif

