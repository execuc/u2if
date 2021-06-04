#ifndef _INTERFACE_UART_H
#define _INTERFACE_UART_H

#include "PicoInterfacesBoard.h"
#include "StreamedInterface.h"
#include "hardware/uart.h"
extern "C" {
#include "pico/util/queue.h"
}


class Uart : public StreamedInterface {
public:
    Uart(uint uartIndex = 0, uint streamBufferSize=512);
    virtual ~Uart();

    CmdStatus process(uint8_t const *cmd, uint8_t response[64]);
    CmdStatus task(uint8_t response[64]);

protected:
    CmdStatus init(uint8_t const *cmd);
    CmdStatus deInit();
    CmdStatus write(const uint8_t *cmd);
    CmdStatus read(const uint8_t *cmd, uint8_t *response);
    uint8_t getInstIndex();
    // TODO
    //CmdStatus writeFromUart(const uint8_t *cmd);
    //CmdStatus readFromUart(const uint8_t *cmd, uint8_t *response);

    queue_t _rxUartQueue;
    static const uint RX_REPORT_QUEUE_SIZE = 512;
    uart_inst_t *_uartInst;
    uint _txGP;
    uint _rxGP;
};


#endif

