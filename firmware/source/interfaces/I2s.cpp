#include "I2s.h"

#include <string.h>
#include <algorithm>

#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

#include "audio_i2s.pio.h"

static const PIO _pio = pio1;
static const uint _sm = 1;
static int _dmaChannel;

static I2s* _i2s = nullptr;

static void dma_handler() {
    _i2s->handleDmaIrq();
    // Clear the interrupt request.
    dma_hw->ints1 = 1u << _dmaChannel;
}

void I2s::handleDmaIrq()
{
    releaseOutputBuffer();
    Buffer* pBuffer = acquireOutputBuffer();
    if(pBuffer != nullptr) {
        dma_channel_transfer_from_buffer_now(_dmaChannel, pBuffer->vector.data(), pBuffer->size/4);
    } else {
        //printf("finished\n") ;
    }
}

I2s::I2s(uint32_t bufferSizeInBytes, uint32_t bufferCount)
    : BufferedInterface(bufferSizeInBytes, bufferCount), _inputState(INPUT_STATE::IDLE){
    _i2s = this;
    initDma();
}

I2s::~I2s() {
}

CmdStatus I2s::process(uint8_t const *cmd, uint8_t response[64]) {
    CmdStatus status = CmdStatus::NOT_CONCERNED;

    if(cmd[0] == Report::ID::I2S_INIT) {
        status = init(cmd);
    } else if(cmd[0] == Report::ID::I2S_DEINIT) {
        status = deinit(cmd);
    } else if(cmd[0] == Report::ID::I2S_SET_FREQ) {
        status = setFreq(cmd);
    } else if(cmd[0] == Report::ID::I2S_WRITE_BUFFER) {
        status = writeBuffer(cmd, response);
    }

    return status;
}

CmdStatus I2s::task(uint8_t response[64]) {

    CmdStatus status = CmdStatus::NOT_CONCERNED;
    if(_inputState == INPUT_STATE::IDLE) {
        status = CmdStatus::NOT_CONCERNED;
    } else if(_inputState == INPUT_STATE::WAIT_INPUT) {
        if(streamRxRead()) {
            releaseInputBuffer();
            startDma();
            _inputState = INPUT_STATE::IDLE;

            // send ACK
            response[0] = Report::ID::I2S_WRITE_BUFFER;
            status = CmdStatus::OK;
        } else {
            status = CmdStatus::NOT_FINISHED;
        }
    }
    return status;
}


CmdStatus I2s::init(uint8_t const *cmd) {
    (void)cmd;
    if(getInterfaceState() == InterfaceState::INTIALIZED) {
        return CmdStatus::NOK;
    }
    resetBuffers();
    _offsetProgram = pio_add_program(_pio, &audio_i2s_program);
    const uint data_pin = U2IF_I2S_SD;
    const uint clock_pin_base = U2IF_I2S_CLK;
    audio_i2s_program_init(_pio, _sm, _offsetProgram, data_pin, clock_pin_base);
    update_pio_frequency(10000); // set default speed ?
    pio_sm_set_enabled(_pio, _sm, true);

    setInterfaceState(InterfaceState::INTIALIZED);
    return CmdStatus::OK;
}

CmdStatus I2s::deinit(uint8_t const *cmd) {
    (void)cmd;
    if(getInterfaceState() == InterfaceState::NOT_INITIALIZED) {
        return CmdStatus::OK; // do nothing
    }

    resetBuffers();
    dma_channel_abort(_dmaChannel);
    dma_channel_wait_for_finish_blocking(_dmaChannel);
    pio_sm_set_enabled(_pio, _sm, false);
    resetBuffers();

    pio_remove_program(_pio, &audio_i2s_program, _offsetProgram);


    setInterfaceState(InterfaceState::NOT_INITIALIZED);
    return CmdStatus::OK;
}

CmdStatus I2s::setFreq(uint8_t const *cmd) {
    if(getInterfaceState() != InterfaceState::INTIALIZED) {
        return CmdStatus::NOK;
    }
    const uint32_t freq = convertBytesToUInt32(&cmd[1]);
    bool ret = update_pio_frequency(freq);
    return ret ? CmdStatus::OK : CmdStatus::NOK;
}

bool I2s::update_pio_frequency(uint32_t sample_freq) {
    //printf("setting pio freq %d\n", (int) sample_freq);
    uint32_t system_clock_frequency = clock_get_hz(clk_sys);
    if(!(system_clock_frequency < 0x40000000))
        return false;

    uint32_t divider = system_clock_frequency * 4 / sample_freq; // avoid arithmetic overflow
    //printf("System clock at %u, I2S clock divider 0x%x/256\n", (uint) system_clock_frequency, (uint)divider);
    if(!(divider < 0x1000000))
        return false;
    pio_sm_set_clkdiv_int_frac(_pio, 1, divider >> 8u, divider & 0xffu);
    return true;
}


CmdStatus I2s::writeBuffer(uint8_t const *cmd, uint8_t response[64]) {
    const uint32_t nbBytes = convertBytesToUInt32(&cmd[1]);
    response[2] = 0x00;

    if(nbBytes > _bufSize) {
        response[2] = 0x01;
        return CmdStatus::NOK;
    }

    Buffer *pBuffer = acquireInputBuffer();
    if(!pBuffer) {
        response[2] = 0x02;
        return CmdStatus::NOK;
    }
    pBuffer->progressSize = 0;
    pBuffer->size = nbBytes;

    flushStreamRx();
    _inputState = INPUT_STATE::WAIT_INPUT;
    return CmdStatus::OK;
}

void I2s::initDma() {
    _dmaChannel = dma_claim_unused_channel(true);
    dma_channel_config dmaConfig = dma_channel_get_default_config(_dmaChannel);
    channel_config_set_transfer_data_size(&dmaConfig, DMA_SIZE_32);
    channel_config_set_read_increment(&dmaConfig, true);
    channel_config_set_dreq(&dmaConfig, DREQ_PIO1_TX1);
    dma_channel_configure(_dmaChannel,
            &dmaConfig,
            &_pio->txf[1],  // Write address (only need to set this once)
            NULL,           // Don't provide a read address yet
            0,              // Write the same value many times, then halt and interrupt
            false           // Don't start yet
        );

    // Tell the DMA to raise IRQ line 0 when the channel finishes a block
    dma_channel_set_irq1_enabled(_dmaChannel, true);

    // Configure the processor to run dma_handler() when DMA IRQ 0 is asserted
    irq_set_exclusive_handler(DMA_IRQ_1, dma_handler);
    irq_set_enabled(DMA_IRQ_1, true);
}

void I2s::startDma(){
    Buffer *pBuffer = acquireOutputBuffer();
    if(pBuffer == nullptr) {
        return;
    }
    //printf("Start dma for %d 32bit value\n", pBuffer->size/4);
    dma_channel_transfer_from_buffer_now(_dmaChannel, pBuffer->vector.data(), pBuffer->size/4);
    /*printf("manual send\n");
    for(int it = 0; it<(pBuffer->size/4); it++) {
        printf("manual send(%d): 0x%04Xn", it, pBuffer->vector.data()[it]);
        pio_sm_put_blocking(_pio, 1, pBuffer->vector.data()[it]);
    }*/
}

