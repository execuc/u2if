#include "I2cMaster.h"
#include "string.h"


I2CMaster::I2CMaster(uint8_t i2cIndex, uint streamBufferSize = 512)
    : StreamedInterface(streamBufferSize),
      _i2cInst(i2cIndex == 0 ? i2c0 : i2c1),
      _sdaGP(i2cIndex == 0 ? Pin::ID::GP4_I2C0_SDA : Pin::ID::GP14_I2C1_SDA),
      _sclGP(i2cIndex == 0 ? Pin::ID::GP5_I2C0_SCL : Pin::ID::GP15_I2C1_SCL),
      _currentStreamAddress(0) {

}

I2CMaster::~I2CMaster() {

}

uint8_t I2CMaster::getInstIndex() {
    return static_cast<uint8_t>(i2c_hw_index(_i2cInst));
}

CmdStatus I2CMaster::process(uint8_t const *cmd, uint8_t response[64]) {
    CmdStatus status = CmdStatus::NOT_CONCERNED;
    const uint i2cIndex = getInstIndex();

    if(cmd[0] == (Report::ID::I2C0_INIT + i2cIndex * 0x10)) {
        status = init(cmd);
    } else if(cmd[0] == (Report::ID::I2C0_DEINIT + i2cIndex * 0x10) ) {
        status = deInit();
    } else if(cmd[0] == (Report::ID::I2C0_WRITE + i2cIndex * 0x10)) {
        status = write(cmd);
    } else if(cmd[0] == (Report::ID::I2C0_READ + i2cIndex * 0x10)) {
        status = read(cmd, response);
    } else if(cmd[0] == (Report::ID::I2C0_WRITE_FROM_UART + i2cIndex * 0x10)) {
        status = writeFromUart(cmd);
    }

    return status;
}

CmdStatus I2CMaster::task(uint8_t response[64]) {
    if(_totalRemainingBytesToSend == 0)
        return CmdStatus::NOT_CONCERNED;

    bool error = false;
    while(_totalRemainingBytesToSend > 0 && streamRxAvailableSize() && !error){
        streamRxRead();
        uint nbBytes = std::min(_totalRemainingBytesToSend, _bufferRx.size());
        bool noStop = (_totalRemainingBytesToSend - nbBytes) > 0;

        int nbWritten = i2c_write_blocking(
            _i2cInst,
            _currentStreamAddress, // addr
            _bufferRx.getDataPtr8(), //src
            nbBytes, //len
            noStop
        );
        _i2cInst->restart_on_next = 0;

        if(nbWritten != nbBytes) {
            error = true;
        } else {
            _totalRemainingBytesToSend -= static_cast<uint>(nbWritten);
        }
        _bufferRx.setSize(0);

        break; // break ou pas ? est ce qu'on essaye de tout envoyer sans redonner la main a l'event loop ou on fait le contraire ?
    }

    if(error || _totalRemainingBytesToSend == 0) {
        _totalRemainingBytesToSend = 0;
        _currentStreamAddress = 0;
        response[0] = Report::ID::I2C0_WRITE_FROM_UART + (getInstIndex() * 0x10);
        //memset(&response[2], 0, 62);
        _i2cInst->restart_on_next = 1;
        return error ? CmdStatus::NOK : CmdStatus::OK;
    }
    return CmdStatus::NOT_FINISHED;
}

CmdStatus I2CMaster::init(uint8_t const *cmd) {
    uint32_t baudrate = convertBytesToUInt32(&cmd[2]);
    //printf("i2c baudrate %d kbaud %d %d %d\n", baudrate, report[2], report[3], sizeof(int));
    i2c_init(_i2cInst, baudrate);
    gpio_set_function(_sdaGP, GPIO_FUNC_I2C);
    gpio_set_function(_sclGP, GPIO_FUNC_I2C);
    if(cmd[1]) {
        gpio_pull_up(_sdaGP);
        gpio_pull_up(_sclGP);
    }
    setInterfaceState(InterfaceState::INTIALIZED);
    return CmdStatus::OK;
}

CmdStatus I2CMaster::deInit() {
    i2c_deinit(_i2cInst);
    gpio_disable_pulls(_sdaGP);
    gpio_disable_pulls(_sclGP);
    setInterfaceState(InterfaceState::NOT_INITIALIZED);
    return CmdStatus::OK;
}



CmdStatus I2CMaster::write(const uint8_t *cmd){
    uint nbytes = convertBytesToUInt32(&cmd[3]);
    //printf("i2c write addr=%d size=%d => ", report[1], nbytes);
    
    bool noStop = cmd[2] == 0x01 ? false : true;
    bool over = false;
    if(nbytes > (HID_CMD_SIZE - 7)) {
        noStop = true;
        nbytes = HID_CMD_SIZE - 7;
        over = true;
    }
    
    /*for(int it = 0; it < nbytes; it++) 
    {  
        printf("0x%02x ", report[4 + it]);
    }*/

    int nbWritten = i2c_write_blocking(
        _i2cInst,
        cmd[1], // addr
        cmd + 7, //src
        nbytes, //len
        noStop
    );
    //printf("i2c write res=%d\n", nbWritten);
    if(nbWritten ==  PICO_ERROR_GENERIC || nbWritten != static_cast<int>(nbytes))
        return CmdStatus::NOK;
    if(over)
        _i2cInst->restart_on_next = 0;
    return CmdStatus::OK;
}

CmdStatus I2CMaster::read(const uint8_t *report, uint8_t *ret){
    //printf("i2c read addr=%d size=%d\n", report[1], report[3]);
    int nbRead = i2c_read_blocking(
        _i2cInst,
        report[1], // addr
        ret + 2, //dst
        report[3], //len
        report[2] == 0x01 ? false : true
    );
    /*printf("i2c read res=%d", nbRead);
    if(nbRead > 0) {
        printf(" => ");
        for(int it = 0; it < nbRead; it++) 
        {  
            printf("0x%02x ", ret[2+it]);
        }
    }
    printf("\n");*/
    if(nbRead ==  PICO_ERROR_GENERIC || nbRead != report[3])
        return CmdStatus::NOK;
    return CmdStatus::OK;
}

CmdStatus I2CMaster::writeFromUart(const uint8_t *cmd){
    flushStreamRx();
    _totalRemainingBytesToSend = convertBytesToUInt32(&cmd[2]);
    _currentStreamAddress = cmd[1];
    return CmdStatus::OK;
}
