/*
 * Use code of MicroPython (https://github.com/micropython/micropython/blob/master/ports/rp2/machine_pwm.c)
 * See License file
 */

#include "Pwm.h"

#include "hardware/pwm.h"
#include "hardware/clocks.h"

Pwm::Pwm() {
    setInterfaceState(InterfaceState::INTIALIZED);
}

Pwm::~Pwm() {

}

CmdStatus Pwm::process(uint8_t const *cmd, uint8_t response[64]) {
    CmdStatus status = CmdStatus::NOT_CONCERNED;

    if(cmd[0] == Report::ID::PWM_INIT_PIN) {
        status = initPwm(cmd, response);
    } else if(cmd[0] == Report::ID::PWM_DEINIT_PIN) {
        status = deInitPwm(cmd);
    } else if (cmd[0] == Report::ID::PWM_SET_FREQ) {
        status = setFreq(cmd, response);
    } else if (cmd[0] == Report::ID::PWM_GET_FREQ) {
        status = getFreq(cmd, response);
    } else if (cmd[0] == Report::ID::PWM_SET_DUTY_U16) {
        status = setDutyU16(cmd);
    } else if (cmd[0] == Report::ID::PWM_GET_DUTY_U16) {
        status = getDutyU16(cmd, response);
    } else if (cmd[0] == Report::ID::PWM_SET_DUTY_NS) {
        status = setDutyNs(cmd, response);
    } else if (cmd[0] == Report::ID::PWM_GET_DUTY_NS) {
        status = getDutyNs(cmd, response);
    }

    return status;
}

CmdStatus Pwm::task(uint8_t response[64]) {
    (void)response;

    CmdStatus status = CmdStatus::NOT_CONCERNED;
    return status;
}

CmdStatus Pwm::initPwm(uint8_t const *cmd, uint8_t response[64]) {
    const uint8_t gpio = cmd[1];
    uint sliceNb = pwm_gpio_to_slice_num(gpio);
    uint channel = pwm_gpio_to_channel(gpio);

    response[2] = static_cast<uint8_t>(gpio);
    response[3] = static_cast<uint8_t>(sliceNb);
    response[4] = static_cast<uint8_t>(channel);

    Slice &slice = _sliceArray[sliceNb];
    if(slice.isGpioChannelUsed(channel)) {
        response[5] = 0x01;
        return CmdStatus::NOK;
    }

    gpio_set_function(gpio, GPIO_FUNC_PWM);
    slice.setGpioChannel(channel, gpio);
    return CmdStatus::OK;
}

CmdStatus Pwm::deInitPwm(uint8_t const *cmd) {
    const uint8_t gpio = cmd[1];
    uint sliceNb = pwm_gpio_to_slice_num(gpio);
    uint channel = pwm_gpio_to_channel(gpio);

    Slice &slice = _sliceArray[sliceNb];
    if(slice.isGpioChannelUsed(channel)) {
        slice.unsetGpioChannel(channel);
    }

    if(slice.isFree()) {
       pwm_set_enabled(sliceNb, false);
    }

    return CmdStatus::OK;
}

CmdStatus Pwm::setFreq(uint8_t const *cmd, uint8_t response[64]) {
    bool error = false;
    const uint8_t gpio = cmd[1];
    uint sliceNb = pwm_gpio_to_slice_num(gpio);
    //uint channel = pwm_gpio_to_channel(gpio);

    response[2] = gpio;
    response[3] = 0x00;
    Slice &slice = _sliceArray[sliceNb];
    const uint32_t source_hz = clock_get_hz(clk_sys);

    // Set the frequency, making "top" as large as possible for maximum resolution.
    // Maximum "top" is set at 65534 to be able to achieve 100% duty with 65535.
    const uint32_t TOP_MAX = 65534;
    uint32_t freq = convertBytesToUInt32(&cmd[2]);
    uint32_t div16_top = 16 * source_hz / freq;
    uint32_t top = 1;
    for (;;) {
        // Try a few small prime factors to get close to the desired frequency.
        if (div16_top >= 16 * 5 && div16_top % 5 == 0 && top * 5 <= TOP_MAX) {
            div16_top /= 5;
            top *= 5;
        } else if (div16_top >= 16 * 3 && div16_top % 3 == 0 && top * 3 <= TOP_MAX) {
            div16_top /= 3;
            top *= 3;
        } else if (div16_top >= 16 * 2 && top * 2 <= TOP_MAX) {
            div16_top /= 2;
            top *= 2;
        } else {
            break;
        }
    }
    if (div16_top < 16) {
        response[3] = 0x03;
        error = true;
    } else if (div16_top >= 256 * 16) {
        response[3] = 0x02;
        error = true;
    }

    if(!error && slice.isFreqComputed && ( pwm_hw->slice[sliceNb].div != div16_top || pwm_hw->slice[sliceNb].top != top )) {
        response[3] = 0x01;
        error = true;
    }

    if(!error){
        slice.isFreqComputed = true;
        pwm_hw->slice[sliceNb].div = div16_top;
        pwm_hw->slice[sliceNb].top = top;
        return CmdStatus::OK;
    } else {
        return CmdStatus::NOK;
    }
}

CmdStatus Pwm::getFreq(uint8_t const *cmd, uint8_t response[64]){
    const uint8_t gpio = cmd[1];
    uint slice = pwm_gpio_to_slice_num(gpio);
    const uint32_t source_hz = clock_get_hz(clk_sys);
    const uint32_t div16 = pwm_hw->slice[slice].div;
    const uint32_t top = pwm_hw->slice[slice].top;
    const uint32_t pwm_freq = 16 * source_hz / div16 / top;

    response[2] = gpio;
    convertUInt32ToBytes(pwm_freq, &response[3]);
    return CmdStatus::OK;
}

CmdStatus Pwm::setDutyU16(uint8_t const *cmd) {
    const uint8_t gpio = cmd[1];
    uint sliceNb = pwm_gpio_to_slice_num(gpio);
    uint channel = pwm_gpio_to_channel(gpio);

    uint16_t duty_u16 = convertBytesToUInt16(&cmd[2]);
    const uint32_t top = pwm_hw->slice[sliceNb].top;
    uint32_t cc = duty_u16 * (top + 1) / 65535;
    pwm_set_chan_level(sliceNb, channel, cc);
    pwm_set_enabled(sliceNb, true);
    return CmdStatus::OK;
}

CmdStatus Pwm::getDutyU16(uint8_t const *cmd, uint8_t response[64]) {
    const uint8_t gpio = cmd[1];
    uint sliceNb = pwm_gpio_to_slice_num(gpio);
    uint channel = pwm_gpio_to_channel(gpio);

    const uint32_t top = pwm_hw->slice[sliceNb].top;
    uint32_t cc = pwm_hw->slice[sliceNb].cc;
    cc = (cc >> (channel ? PWM_CH0_CC_B_LSB : PWM_CH0_CC_A_LSB)) & 0xffff;
    uint32_t duty_u16 = cc * 65535 / (top + 1);

    response[2] = gpio;
    convertUInt16ToBytes(static_cast<uint16_t>(duty_u16), &response[3]);
    return CmdStatus::OK;
}

CmdStatus Pwm::setDutyNs(uint8_t const *cmd, uint8_t response[64]) {
    bool error = false;
    const uint8_t gpio = cmd[1];
    uint sliceNb = pwm_gpio_to_slice_num(gpio);
    uint channel = pwm_gpio_to_channel(gpio);

    uint32_t source_hz = clock_get_hz(clk_sys);
    uint32_t slice_hz = 16 * source_hz / pwm_hw->slice[sliceNb].div;

    uint32_t duty_ns = convertBytesToUInt32(&cmd[2]);

    uint32_t cc = (uint64_t)duty_ns * slice_hz / 1000000000ULL;
    if (cc > 65535) {
        error = true;
        response[3] = 0x01;
    }
    response[2] = gpio;
    if(!error) {
        pwm_set_chan_level(sliceNb, channel, cc);
        pwm_set_enabled(sliceNb, true);
        return CmdStatus::OK;
    } else {
        return CmdStatus::NOK;
    }
}

CmdStatus Pwm::getDutyNs(uint8_t const *cmd, uint8_t response[64]) {
    const uint8_t gpio = cmd[1];
    uint sliceNb = pwm_gpio_to_slice_num(gpio);
    uint channel = pwm_gpio_to_channel(gpio);

    uint32_t source_hz = clock_get_hz(clk_sys);
    uint32_t slice_hz = 16 * source_hz / pwm_hw->slice[sliceNb].div;

    uint32_t cc = pwm_hw->slice[sliceNb].cc;
    cc = (cc >> (channel ? PWM_CH0_CC_B_LSB : PWM_CH0_CC_A_LSB)) & 0xffff;
    uint64_t dutyNs = static_cast<uint64_t>(cc) * 1000000000ULL / slice_hz;

    response[2] = gpio;
    convertUInt32ToBytes(static_cast<uint32_t>(dutyNs), &response[3]);
    return CmdStatus::OK;
}
