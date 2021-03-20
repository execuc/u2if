#include "ModeActivity.h"

#include "hardware/gpio.h"

// Activity led
const uint8_t LED_PIN = 25;
DeviceState ModeActivity::_sDeviceState = DeviceState::NOT_MOUNTED;
repeating_timer_t ModeActivity::_sLedTimer;
uint32_t ModeActivity::_sBlinkPeriodCount;
// CONST
const int32_t LED_TASK_PERIODS_MS = 50;
const uint32_t NOT_MOUNTED_STATE_PERIODS_MS = 1000;
const uint32_t SUSPENDED_STATE_PERIODS_MS = 2500;


ModeActivity::ModeActivity() {
    _sDeviceState = DeviceState::NOT_MOUNTED;
    _sBlinkPeriodCount = 0;
}

ModeActivity::~ModeActivity() {

}

void ModeActivity::init() {
    // init activity led and timer
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, true);
    gpio_put(LED_PIN, false);
    add_repeating_timer_ms(-LED_TASK_PERIODS_MS, ledTask, NULL, &_sLedTimer);
}

void ModeActivity::setDeviceState(DeviceState deviceState) {
    _sDeviceState = deviceState;
    if(_sDeviceState == DeviceState::MOUNTED) {
        _sBlinkPeriodCount = 10;
    }
}

DeviceState ModeActivity::getDeviceState() {
    return _sDeviceState;
}

void ModeActivity::setBlinking() {
    _sBlinkPeriodCount = 2;
}

void ModeActivity::setBlinkingInfinite() {
    _sBlinkPeriodCount = 0xFFFFFFFF;
}

bool ModeActivity::ledTask(repeating_timer_t *rt) {
    (void)rt;
    static bool led_state = false;
    static uint32_t periodCounter = 0;

    if(_sDeviceState == DeviceState::SUSPENDED || _sDeviceState == DeviceState::NOT_MOUNTED) {
        led_state = !!(periodCounter == 0);

        if(++periodCounter > (_sDeviceState == DeviceState::SUSPENDED ? SUSPENDED_STATE_PERIODS_MS/LED_TASK_PERIODS_MS : NOT_MOUNTED_STATE_PERIODS_MS/LED_TASK_PERIODS_MS))
            periodCounter = 0;
    } else if(_sDeviceState == DeviceState::MOUNTED) {
        if(_sBlinkPeriodCount > 0) {
            led_state = 1 - led_state;
            _sBlinkPeriodCount--;
        } else {
            led_state = false;
        }
    } else {
        led_state = 1 - led_state;
    }
    gpio_put(LED_PIN, led_state);

    return true;
}
