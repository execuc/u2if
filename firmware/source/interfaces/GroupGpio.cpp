#include "GroupGpio.h"
#include "hardware/gpio.h"


GroupGpio::GroupGpio() {
    setInterfaceState(InterfaceState::INTIALIZED);
}

GroupGpio::~GroupGpio() {
}

CmdStatus GroupGpio::process(uint8_t const *cmd, uint8_t response[64]) {
    CmdStatus status = CmdStatus::NOT_CONCERNED;

    if(cmd[0] == Report::ID::GROUP_GPIO_SET_VALUES) {
        status = setMaskPins(cmd);
    } else if(cmd[0] == Report::ID::GROUP_GPIO_GET_ALL_VALUES) {
        status = getAllPins(cmd, response);
    }
    

    return status;
}

CmdStatus GroupGpio::task(uint8_t response[64]) {
    (void)response;

    CmdStatus status = CmdStatus::NOT_CONCERNED;
    return status;
}

/*
CmdStatus GroupGpio::initMasksPins(uint8_t const *cmd) {
    const uint32_t gpioMask = convertBytesToUInt32(&cmd[1]);
    // not used because inputs are not yet managed
    // const uint32_t gpioDirs = convertBytesToUInt32(&cmd[5]); 
    // const uint32_t pull = convertBytesToUInt32(&cmd[9]);

    gpio_init_mask(gpioMask);
    gpio_set_dir_out_masked(gpioMask);

    return CmdStatus::OK;
}*/

CmdStatus GroupGpio::setMaskPins(uint8_t const *cmd) {
    const uint32_t gpioMask = convertBytesToUInt32(&cmd[1]);
    const uint32_t gpioValues = convertBytesToUInt32(&cmd[5]);

    gpio_put_masked(gpioMask, gpioValues);
    return CmdStatus::OK;
}

CmdStatus GroupGpio::getAllPins(uint8_t const *cmd, uint8_t response[64]) {
    uint32_t gpioValues = gpio_get_all();
	convertUInt32ToBytes(gpioValues, &response[2]);
    return CmdStatus::OK;
}
