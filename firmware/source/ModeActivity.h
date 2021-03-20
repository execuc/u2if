#ifndef _MODE_ACTIVITY_H
#define _MODE_ACTIVITY_H

#include "pico/time.h"

// USB Device state
enum DeviceState {
    NOT_MOUNTED = 1,
    MOUNTED = 2,
    SUSPENDED = 3
};

class ModeActivity {
public:
    ModeActivity();
    ~ModeActivity();

    void init();
    static void setDeviceState(DeviceState deviceState);
    static DeviceState getDeviceState();
    static void setBlinking();
    static void setBlinkingInfinite();

protected:
    static bool ledTask(repeating_timer_t *rt);
    static DeviceState _sDeviceState;
    static repeating_timer_t _sLedTimer;
    static uint32_t _sBlinkPeriodCount;
};


#endif

