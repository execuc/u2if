#include "Hub75.h"
#include "string.h"
#include <algorithm>
#include <math.h>
#include "pico/multicore.h"
#include "hardware/pio.h"
#include "hub75.pio.h"


#define DATA_BASE_PIN 0
#define DATA_N_PINS 6
#define ROWSEL_BASE_PIN 6
//#define ROWSEL_N_PINS 4
#define CLK_PIN 11
#define STROBE_PIN 12
#define OEN_PIN 13


static uint32_t * nextImg[2] = {nullptr, nullptr};

uint8_t Hub75::WIDTH = 0;
uint8_t Hub75::HEIGHT = 0;

void hub75_core() {

    PIO pio = pio1;
    uint sm_data = 2;
    uint sm_row = 3;

    int ROWSEL_N_PINS = (int)log2(Hub75::HEIGHT >> 1);

    uint data_prog_offs = pio_add_program(pio, &hub75_data_rgb888_program);
    uint row_prog_offs = pio_add_program(pio, &hub75_row_program);
    hub75_data_rgb888_program_init(pio, sm_data, data_prog_offs, DATA_BASE_PIN, CLK_PIN);
    hub75_row_program_init(pio, sm_row, row_prog_offs, ROWSEL_BASE_PIN, ROWSEL_N_PINS, STROBE_PIN);

    uint32_t currentIndex = 0;
    while (1) {
        if(multicore_fifo_rvalid())
            currentIndex = multicore_fifo_pop_blocking();
        uint32_t *img = nextImg[currentIndex];
        for (int rowsel = 0; rowsel < (1 << ROWSEL_N_PINS); ++rowsel) {
            uint32_t* gc_row[2];
            gc_row[0] = &img[rowsel * Hub75::WIDTH];
            gc_row[1] = &img[((1u << ROWSEL_N_PINS) + rowsel) * Hub75::WIDTH];
            for (int bit = 0; bit < 8; ++bit) {
                hub75_data_rgb888_set_shift(pio, sm_data, data_prog_offs, bit);
                for (int x = 0; x < Hub75::WIDTH; ++x) {
                    pio_sm_put_blocking(pio, sm_data, gc_row[0][x]);
                    pio_sm_put_blocking(pio, sm_data, gc_row[1][x]);
                }
                // Dummy pixel per lane
                pio_sm_put_blocking(pio, sm_data, 0);
                pio_sm_put_blocking(pio, sm_data, 0);
                // SM is finished when it stalls on empty TX FIFO
                hub75_wait_tx_stall(pio, sm_data);
                // Also check that previous OEn pulse is finished, else things can get out of sequence
                hub75_wait_tx_stall(pio, sm_row);

                // Latch row data, pulse output enable for new row.
                pio_sm_put_blocking(pio, sm_row, rowsel | (100u * (1u << bit) << 5));
            }
        }
    }

}


Hub75::Hub75(uint streamBufferSize)
    : StreamedInterface(streamBufferSize, true),
      _internalState(INTERNAL_STATE::IDLE) {

    nextImg[0] = _bufferRx.getDataPtr32();
    nextImg[1] = _bufferRx2.getDataPtr32();
}

Hub75::~Hub75() {

}

CmdStatus Hub75::process(uint8_t const *cmd, uint8_t response[64]) {
    CmdStatus status = CmdStatus::NOT_CONCERNED;

    if(cmd[0] == Report::ID::HUB75_INIT) {
        status = init(cmd, response);
    } else if(cmd[0] == Report::ID::HUB75_DEINIT) {
        status = deInit();
    } else if(cmd[0] == Report::ID::HUB75_WRITE) {
        status = write(cmd, response);
    }

    return status;
}

CmdStatus Hub75::task(uint8_t response[64]) {
    CmdStatus status = CmdStatus::NOT_CONCERNED;

    if(_internalState == INTERNAL_STATE::IDLE) {
        status = CmdStatus::NOT_CONCERNED;
    } else if(_internalState == INTERNAL_STATE::WAIT_PIXELS && getBuffer().size() < _totalRemainingBytesToSend) {
        streamRxRead();
        status = CmdStatus::NOT_FINISHED;
    } else if(_internalState == INTERNAL_STATE::WAIT_PIXELS) {// && getBuffer().size() >= _totalRemainingBytesToSend)
        _internalState = INTERNAL_STATE::IDLE;
        multicore_fifo_push_blocking(getCurrentBufferIndex());
        _totalRemainingBytesToSend = 0;
        // send ACK
        response[0] = Report::ID::HUB75_WRITE;
        status = CmdStatus::OK;
    }
    return status;
}

// HUB75
// | HUB75_INIT | WIDTH | HEIGHT |
CmdStatus Hub75::init(uint8_t const *cmd, uint8_t response[64]) {
    (void)response;
    if(getInterfaceState() == InterfaceState::INTIALIZED) {
        return CmdStatus::NOK;
    }

    Hub75::WIDTH = cmd[1];
    Hub75::HEIGHT = cmd[2];

    if((Hub75::WIDTH * Hub75::HEIGHT) > HUB75_MAX_LEDS) {
        return CmdStatus::NOK;
    } else if(!((Hub75::HEIGHT != 0) && ((Hub75::HEIGHT &(Hub75::HEIGHT - 1)) == 0))) {
        // Height is not a power of 2
        return CmdStatus::NOK;
    }
    
    memset(_bufferRx.getDataPtr8(), 0, Hub75::HEIGHT * Hub75::WIDTH * 4);
    memset(_bufferRx2.getDataPtr8(), 0, Hub75::HEIGHT * Hub75::WIDTH * 4);

    multicore_launch_core1(hub75_core);
    setInterfaceState(InterfaceState::INTIALIZED);
    return CmdStatus::OK;
}

CmdStatus Hub75::deInit() {
    multicore_reset_core1(); // Seems to not working, must stop PIO before ?
    setInterfaceState(InterfaceState::NOT_INITIALIZED);
    return CmdStatus::OK;
}

// | HUB75_WRITE | NB_BYTES[4] L.Endian (WIDTH * HEIGHT *3)| => First | HUB75_WRITE | CmdStatus::OK|NOK | 
// ... and after the CDC stream (when transfer to led starting) | HUB75_WRITE | CmdStatus::OK |
CmdStatus Hub75::write(const uint8_t *cmd, uint8_t response[64]){
    const uint32_t nbBytes = convertBytesToUInt32(&cmd[1]);
    response[2] = 0x00;
    if(_internalState != INTERNAL_STATE::IDLE){
        response[2] = 0x02;
        return CmdStatus::NOK;
    } else if(nbBytes > (Hub75::WIDTH * Hub75::HEIGHT * 4)) {
        _totalRemainingBytesToSend = 0;
        response[2] = 0x01;
        return CmdStatus::NOK;
    }

    switchBuffer();
    getBuffer().setSize(0);
    flushStreamRx();

    _totalRemainingBytesToSend = nbBytes;
    _internalState = INTERNAL_STATE::WAIT_PIXELS;
    
    return CmdStatus::OK;
}
