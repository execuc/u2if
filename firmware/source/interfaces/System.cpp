#include "System.h"

#include "hardware/watchdog.h"
#include "pico/unique_id.h"

System::System()
 : _needReset(false), count(0){
    setInterfaceState(InterfaceState::INTIALIZED);
}

System::~System() {

}

CmdStatus System::process(uint8_t const *cmd, uint8_t response[64]) {
    CmdStatus status = CmdStatus::NOT_CONCERNED;

    if(cmd[0] == Report::ID::SYS_RESET) {
        count = 0;
        _needReset = true;
        status = CmdStatus::OK;
    } else if(cmd[0] == Report::ID::SYS_GET_SN) {
        pico_unique_board_id_t board_id;
        pico_get_unique_board_id(&board_id);
        for (uint i = 0; i < PICO_UNIQUE_BOARD_ID_SIZE_BYTES; ++i) {
            response[2+i] = board_id.id[i];
        }
        status = CmdStatus::OK;
    } else if(cmd[0] == Report::ID::SYS_GET_VN) {
        response[2] = PROJECT_VER_MAJOR;
        response[3] = PROJECT_VER_MINOR;
        response[4] = PROJECT_VER_PATCH;
        status = CmdStatus::OK;
    }

    return status;
}

CmdStatus System::task(uint8_t response[64]) {
    (void)response;
    CmdStatus status = CmdStatus::NOT_CONCERNED;

    if(_needReset) {
        count++; // give some time in event loop to send System ACK
    }

    if(_needReset && count > 10) {
         watchdog_enable(10, 1);
         while(1);
    }

    return status;
}

