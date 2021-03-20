#ifndef _INTERFACE_I2C_MASTER_H
#define _INTERFACE_I2C_MASTER_H

#include "PicoInterfacesBoard.h"
#include "StreamedInterface.h"
#include "hardware/i2c.h"


class I2CMaster : public StreamedInterface {
public:
    I2CMaster(uint8_t i2cIndex, uint streamBufferSize);
    virtual ~I2CMaster();

    CmdStatus process(uint8_t const *cmd, uint8_t response[64]);
    CmdStatus task(uint8_t response[64]);

protected:
    CmdStatus init(uint8_t const *cmd);
    CmdStatus deInit();
    CmdStatus write(const uint8_t *cmd);
    CmdStatus writeFromUart(const uint8_t *cmd);
    CmdStatus read(const uint8_t *cmd, uint8_t *ret);
    uint8_t getInstIndex();

    i2c_inst_t *_i2cInst;
    uint _sdaGP;
    uint _sclGP;
    uint8_t _currentStreamAddress;
};


#endif

