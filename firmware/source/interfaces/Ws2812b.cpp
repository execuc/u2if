#include "Ws2812b.h"

#include <string.h>
#include <algorithm>

#include "hardware/dma.h"
#include "hardware/irq.h"
#include "ws2812.pio.h"

static PIO _pio = pio0;
static int _dmaChannel;
static bool _dmaInProgress = false;

void dma_handler() {
    _dmaInProgress = false;
    // Clear the interrupt request.
    dma_hw->ints0 = 1u << _dmaChannel;
}

Ws2812b::Ws2812b(uint maxLeds)
    : StreamedInterface(maxLeds * 4 +1 /**/), _maxLeds(maxLeds), _internalState(INTERNAL_STATE::IDLE) {
}

Ws2812b::~Ws2812b() {
}

CmdStatus Ws2812b::process(uint8_t const *cmd, uint8_t response[64]) {
    CmdStatus status = CmdStatus::NOT_CONCERNED;

    if(cmd[0] == Report::ID::WS2812B_INIT) {
        status = init(cmd);
    } else if(cmd[0] == Report::ID::WS2812B_DEINIT) {
        status = deinit(cmd);
    } else if(cmd[0] == Report::ID::WS2812B_WRITE) {
        status = write(cmd, response);
    }

    return status;
}

CmdStatus Ws2812b::task(uint8_t response[64]) {

    CmdStatus status = CmdStatus::NOT_CONCERNED;
    if(_internalState == INTERNAL_STATE::IDLE) {
        status = CmdStatus::NOT_CONCERNED;
    } else if(_internalState == INTERNAL_STATE::WAIT_PIXELS && _bufferRx.size() < _totalRemainingBytesToSend) {
        streamRxRead();
        status = CmdStatus::NOT_FINISHED;
    } else if(_internalState == INTERNAL_STATE::WAIT_PIXELS) {// && _bufferRx.size() >= _totalRemainingBytesToSend)
        _internalState = INTERNAL_STATE::TRANSFER_IN_PROGRESS;
        startTransfer(_bufferRx.getDataPtr32(), _totalRemainingBytesToSend / 4);
        // send ACK
        response[0] = Report::ID::WS2812B_WRITE;
        status = CmdStatus::OK;
    } else if(_internalState == INTERNAL_STATE::TRANSFER_IN_PROGRESS && _dmaInProgress) {
        status = CmdStatus::NOT_FINISHED;
    } else if(_internalState == INTERNAL_STATE::TRANSFER_IN_PROGRESS) { // !_dmaInProgress
        _internalState = INTERNAL_STATE::TRANSFER_FINISHED;
        status = CmdStatus::NOT_FINISHED;
    } else if(_internalState == INTERNAL_STATE::TRANSFER_FINISHED) {
        _totalRemainingBytesToSend = 0;
        _bufferRx.setSize(0);
        _internalState = INTERNAL_STATE::IDLE;
        status = CmdStatus::NOT_CONCERNED; //ACK was already sent
    }
    return status;
}


CmdStatus Ws2812b::init(uint8_t const *cmd) {
    if(getInterfaceState() == InterfaceState::INTIALIZED) {
        return CmdStatus::NOK;
    }

    // TODO: use free SM
    uint sm = 0;
    uint offset = pio_add_program(_pio, &ws2812_program);
    const uint pinId = cmd[1];
    ws2812_program_init(_pio, sm, offset, pinId, 800000, false);
    // Init dma
    _dmaChannel = dma_claim_unused_channel(true);
    dma_channel_config dmaConfig = dma_channel_get_default_config(_dmaChannel);
    channel_config_set_transfer_data_size(&dmaConfig, DMA_SIZE_32);
    channel_config_set_read_increment(&dmaConfig, true);
    channel_config_set_dreq(&dmaConfig, DREQ_PIO0_TX0);
    dma_channel_configure(_dmaChannel,
            &dmaConfig,
            &pio0->txf[0],  // Write address (only need to set this once)
            NULL,           // Don't provide a read address yet
            0,              // Write the same value many times, then halt and interrupt
            false           // Don't start yet
        );

    // Tell the DMA to raise IRQ line 0 when the channel finishes a block
    dma_channel_set_irq0_enabled(_dmaChannel, true);

    // Configure the processor to run dma_handler() when DMA IRQ 0 is asserted
    irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    irq_set_enabled(DMA_IRQ_0, true);

    setInterfaceState(InterfaceState::INTIALIZED);
    return CmdStatus::OK;
}

CmdStatus Ws2812b::deinit(uint8_t const *cmd) {
    (void)cmd;
    // TODO : implement it
    // Deinit + setInterfaceState(InterfaceState::NOT_INTIALIZED);
    return CmdStatus::OK;
}

CmdStatus Ws2812b::write(uint8_t const *cmd, uint8_t response[64]) {
    const uint32_t nbBytes = convertBytesToUInt32(&cmd[1]);
    response[2] = 0x00;
    if(_internalState != INTERNAL_STATE::IDLE){
        response[2] = 0x02;
        return CmdStatus::NOK;
    } else if(nbBytes > (_maxLeds * 4)) {
        _totalRemainingBytesToSend = 0;
        response[2] = 0x01;
        return CmdStatus::NOK;
    }

    flushStreamRx();
    _totalRemainingBytesToSend = nbBytes;
    _internalState = INTERNAL_STATE::WAIT_PIXELS;
    return CmdStatus::OK;
}

void Ws2812b::startTransfer(uint32_t *pixelBuf, uint32_t nbPixels) {
    // sync version
    /*for(uint32_t pxIt=0; pxIt < nbPixels; pxIt++) {
        pio_sm_put_blocking(_pio, 0, pixelBuf[pxIt]);
    }
    _internalState = INTERNAL_STATE::TRANSFER_FINISHED;*/

    // Async version
    _dmaInProgress = true;
    // Give the channel a new wave table entry to read from, and re-trigger it
    dma_channel_transfer_from_buffer_now(_dmaChannel, pixelBuf, nbPixels);
}
