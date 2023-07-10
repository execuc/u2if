#include "Gpio.h"

#include <string.h>
#include <algorithm>
#include "hardware/gpio.h"

// à mettre en variables memebres ?
static const uint8_t MAX_BUFFERED_EVENT = 60;
static uint8_t irqEventBuffer[MAX_BUFFERED_EVENT];
static volatile uint8_t irqEventBufferCount = 0;
static critical_section_t critSec;

// Debouncer variables
const uint8_t MAX_PINS = 30;
const uint8_t DEBOUNCE_PERIODS_MS = 1;
const uint8_t PRESS_PERIODS_MS = 3;
const uint8_t RELEASE_PERIODS_MS = 20;
static uint32_t debouncedEvtRisingList = 0x00;
static uint32_t debouncedEvtFallingList = 0x00;
static uint32_t debouncedStateList = 0x00;
static uint8_t debouncerCounterList[MAX_PINS];


void gpioCallback(uint gpio, uint32_t events) {
    uint8_t interfaceEvent = gpio & 0b00111111;
    if(events & GPIO_IRQ_EDGE_RISE)
        interfaceEvent |= IRQ_EVENT::EVENT_RISING << 6;
    if(events & GPIO_IRQ_EDGE_FALL)
        interfaceEvent |= IRQ_EVENT::EVENT_FALLING << 6;

    critical_section_enter_blocking(&critSec);
    if(irqEventBufferCount < MAX_BUFFERED_EVENT) {
        irqEventBuffer[irqEventBufferCount++] = interfaceEvent;
    }
    critical_section_exit(&critSec);
}

// debouncing : http://stackoverflow.com/questions/155071/simple-debounce-routine
bool debounceInput(repeating_timer_t *rt) {
    (void)rt;

    uint32_t rawStatePins = gpio_get_all();
    for(uint8_t gpio=0; gpio < MAX_PINS; gpio++){
        bool risingEv = !!(debouncedEvtRisingList & (1ul << gpio));
        bool fallingEv = !!(debouncedEvtFallingList & (1ul << gpio));
        if( !fallingEv && !risingEv)
            continue;

        bool rawState = !!((1ul << gpio) & rawStatePins);
        bool debouncedState = debouncedStateList & (0x01 << gpio);
        if(rawState == debouncedState){
            if(debouncedState)
                debouncerCounterList[gpio] = RELEASE_PERIODS_MS / DEBOUNCE_PERIODS_MS;
            else
                debouncerCounterList[gpio] = PRESS_PERIODS_MS / DEBOUNCE_PERIODS_MS;
        } else {
            // Key has changed - wait for new state to become stable.
            if (--(debouncerCounterList[gpio]) == 0) {
                // Timer expired - accept the change.
                uint8_t interfaceEvent = 0x00;
                if(rawState) {
                    debouncedStateList |= (1ul << gpio);
                    debouncerCounterList[gpio] = RELEASE_PERIODS_MS / DEBOUNCE_PERIODS_MS;
                    if(risingEv)
                        interfaceEvent |= IRQ_EVENT::EVENT_RISING << 6;
                } else {
                    debouncedStateList &= ~(1ul << gpio);
                    debouncerCounterList[gpio] = PRESS_PERIODS_MS / DEBOUNCE_PERIODS_MS;
                    if(fallingEv)
                        interfaceEvent |= IRQ_EVENT::EVENT_FALLING << 6;
                }

                if(interfaceEvent != 0x00) {
                    interfaceEvent |= (gpio & 0b00111111);
                    critical_section_enter_blocking(&critSec);
                    if(irqEventBufferCount < MAX_BUFFERED_EVENT) {
                        irqEventBuffer[irqEventBufferCount++] = interfaceEvent;
                    }
                    critical_section_exit(&critSec);
                }
            }
        }
    }

    return true;
}

Gpio::Gpio() {
    setInterfaceState(InterfaceState::INTIALIZED);
    critical_section_init(&critSec);
    add_repeating_timer_us(-DEBOUNCE_PERIODS_MS * 1000, debounceInput, NULL, &_debounceTimer);
}

Gpio::~Gpio() {
}

CmdStatus Gpio::process(uint8_t const *cmd, uint8_t response[64]) {
    CmdStatus status = CmdStatus::NOT_CONCERNED;

    if(cmd[0] == Report::ID::GPIO_INIT_PIN) {
        status = initPin(cmd);
    } else if(cmd[0] == Report::ID::GPIO_SET_VALUE) {
        status = setPin(cmd);
    } else if(cmd[0] == Report::ID::GPIO_GET_VALUE) {
        status = getPin(cmd, response);
    } else if(cmd[0] == Report::ID::GPIO_SET_IRQ) {
        status = setIrq(cmd);
    } else if(cmd[0] == Report::ID::GPIO_GET_IRQ) {
        status = getIrq(cmd, response);
    }

    return status;
}

CmdStatus Gpio::task(uint8_t response[64]) {
    (void)response;

    CmdStatus status = CmdStatus::NOT_CONCERNED;
    return status;
}


CmdStatus Gpio::initPin(uint8_t const *cmd) {
    const uint gp = cmd[1];
    const uint dir = cmd[2] == 0 ? GPIO_IN : GPIO_OUT;
    gpio_init(gp);
    gpio_set_dir(gp, dir);

    if(dir == GPIO_IN && cmd[3] == 1) {
        gpio_pull_up(gp);
    } else if(dir == GPIO_IN && cmd[3] == 2) {
        gpio_pull_down(gp);
    } else if (dir == GPIO_IN) {
        gpio_disable_pulls(gp);
    }
    //a voir gpio_set_outover(gpio, GPIO_OVERRIDE_INVERT);
    return CmdStatus::OK;
}

CmdStatus Gpio::setPin(uint8_t const *cmd) {
    const uint gp = cmd[1];
    bool value = cmd[2] == 0 ? false : true;
    gpio_put(gp, value);
    return CmdStatus::OK;
}

CmdStatus Gpio::getPin(uint8_t const *cmd, uint8_t response[64]) {
    const uint8_t gp = cmd[1];
    response[2] = gp;
    response[3] = gpio_get(gp);
    return CmdStatus::OK;
}

CmdStatus Gpio::setIrq(uint8_t const *cmd) {
    const uint gpio = cmd[1];
    const uint8_t ev = cmd[2];
    bool debounced = cmd[3] == 0x01 ? true : false;
    uint8_t event_flags = 0;
    if(ev & IRQ_EVENT::EVENT_RISING)
        event_flags |= GPIO_IRQ_EDGE_RISE;
    if(ev & IRQ_EVENT::EVENT_FALLING)
        event_flags |= GPIO_IRQ_EDGE_FALL;

    critical_section_enter_blocking(&critSec);
    // Remove previous irq settings if exists
    gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, false);
    debouncedEvtFallingList &= ~(1ul << gpio);
    debouncedEvtRisingList &= ~(1ul << gpio);

    if(event_flags != 0 && !debounced) {
        gpio_set_irq_enabled_with_callback(gpio, event_flags, true, gpioCallback);
    } else if(event_flags != 0) {
        if(ev & IRQ_EVENT::EVENT_FALLING)
            debouncedEvtFallingList |= 1ul << gpio;
        if(ev & IRQ_EVENT::EVENT_RISING)
            debouncedEvtRisingList |= 1ul << gpio;

        if(gpio_get(gpio))
            debouncedStateList |= (0x01 << gpio);
        else
            debouncedStateList &= ~(1ul << gpio);
    }
    critical_section_exit(&critSec);
    return CmdStatus::OK;
}

// TODO: To rework, it is a simple version with buffer smaller than HID response.
// It is necessary to be able to have larger buffers. Currently they are the size of the HID response.
CmdStatus Gpio::getIrq(uint8_t const *cmd, uint8_t response[64]) {
    (void)cmd;
    static uint32_t eventBufferCopy[MAX_BUFFERED_EVENT];
    static uint8_t eventBufferCountCopy = 0;

    if(eventBufferCountCopy == 0) {
        critical_section_enter_blocking(&critSec);
        eventBufferCountCopy = irqEventBufferCount;
        memcpy(eventBufferCopy, irqEventBuffer, sizeof(uint32_t) * eventBufferCountCopy);
        irqEventBufferCount = 0;
        critical_section_exit(&critSec);
    }

    response[2] = eventBufferCountCopy;
    memcpy(&(response[3]), eventBufferCopy, sizeof(uint8_t) * eventBufferCountCopy);
    eventBufferCountCopy = 0;
    return CmdStatus::OK;
}


