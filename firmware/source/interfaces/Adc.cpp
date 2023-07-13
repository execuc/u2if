#include "Adc.h"
#include "hardware/adc.h"

Adc::Adc() {
    setInterfaceState(InterfaceState::INTIALIZED);
    adc_init();
}

Adc::~Adc() {

}

CmdStatus Adc::process(uint8_t const *cmd, uint8_t response[64]) {
    CmdStatus status = CmdStatus::NOT_CONCERNED;

    if(cmd[0] == Report::ID::ADC_INIT_PIN) {
        status = gpioInit(cmd);
    } else if(cmd[0] == Report::ID::ADC_GET_VALUE) {
        status = getValue(cmd, response);
    }


    return status;
}

CmdStatus Adc::task(uint8_t response[64]) {
    (void)response;
    CmdStatus status = CmdStatus::NOT_CONCERNED;


    return status;
}

int8_t Adc::getAdcIndexFromGpio(uint8_t gpio) {
    if(gpio < 26 || gpio > 29)
        return -1;
    else
        return (static_cast<int8_t>(gpio)-26);
}

CmdStatus Adc::gpioInit(uint8_t const *cmd) {
    const uint8_t gpio = cmd[1];
    const int adcIndex = getAdcIndexFromGpio(gpio);
    if(adcIndex >= 0) {
        adc_gpio_init(gpio);
        return CmdStatus::OK;
    } else {
        return CmdStatus::NOK;
    }
}

CmdStatus Adc::getValue(uint8_t const *cmd, uint8_t response[64]) {
    const uint8_t gpio = cmd[1];
    const int adcIndex = getAdcIndexFromGpio(gpio);

    if(adcIndex >= 0) {
        adc_select_input(static_cast<uint>(adcIndex));
        uint16_t value_u16 = adc_read();
        response[2] = gpio;
        convertUInt16ToBytes(static_cast<uint16_t>(value_u16), &response[3]);
        return CmdStatus::OK;
    } else {
        return CmdStatus::NOK;
    }
}

