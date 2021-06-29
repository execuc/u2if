#ifndef _INTERFACE_HUB75_H
#define _INTERFACE_HUB75_H

#include "PicoInterfacesBoard.h"
#include "StreamedInterface.h"
#include "hardware/uart.h"
extern "C" {
#include "pico/util/queue.h"
}


class Hub75 : public StreamedInterface {
public:
    Hub75(uint streamBufferSize);
    virtual ~Hub75();

    CmdStatus process(uint8_t const *cmd, uint8_t response[64]);
    CmdStatus task(uint8_t response[64]);

protected:
    CmdStatus init(uint8_t const *cmd, uint8_t response[64]);
    CmdStatus deInit();
    CmdStatus write(const uint8_t *cmd, uint8_t response[64]);

    enum INTERNAL_STATE {
        IDLE = 0x00,
        WAIT_PIXELS = 0x01,
        //WAIT_START_TRANSFER = 0x02,
        //TRANSFER_IN_PROGRESS = 0x03,
        //TRANSFER_FINISHED = 0x04
    };

    INTERNAL_STATE _internalState;

    /*CmdStatus write(const uint8_t *cmd);
    CmdStatus read(const uint8_t *cmd, uint8_t *response);
    uint8_t getInstIndex();

    queue_t _rxUartQueue;
    static const uint RX_REPORT_QUEUE_SIZE = 512;
    uart_inst_t *_uartInst;
    uint _txGP;
    uint _rxGP;
    */
public:
   static uint8_t WIDTH;
   static uint8_t HEIGHT;
};


#endif

