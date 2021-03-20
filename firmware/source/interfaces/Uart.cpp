#include "Uart.h"
#include "string.h"
#include <algorithm>


Uart::Uart(uint uartIndex, uint streamBufferSize)
    : StreamedInterface(streamBufferSize) {
    _uart = uartIndex == 0 ? uart0 : uart1;
    setInterfaceState(InterfaceState::NOT_INITIALIZED);
    queue_init(&_rxUartQueue, 1, RX_REPORT_QUEUE_SIZE);
}

Uart::~Uart() {

}


CmdStatus Uart::process(uint8_t const *cmd, uint8_t response[64]) {
    CmdStatus status = CmdStatus::NOT_CONCERNED;

    if(cmd[0] == Report::ID::UART0_INIT) {
        status = init(cmd);
    } else if(cmd[0] == Report::ID::UART0_DEINIT ) {
        status = deInit();
    } else if(cmd[0] == Report::ID::UART0_READ) {
        status = read(cmd, response);
    } else if(cmd[0] == Report::ID::UART0_WRITE) {
        status = write(cmd);
    }

    return status;
}

// TODO : use circular buffer whithout thread sync instead this queue
CmdStatus Uart::task(uint8_t response[64]) {
    (void)response;
    CmdStatus status = CmdStatus::NOT_CONCERNED;

    if(getInterfaceState() == InterfaceState::INTIALIZED && uart_is_readable(_uart)) {
        status = CmdStatus::NOT_FINISHED;
        uint count = std::min(RX_REPORT_QUEUE_SIZE - queue_get_level(&_rxUartQueue), 200u );
        while(uart_is_readable(_uart) && count>0) {
            uint8_t c = uart_getc(_uart);
            queue_try_add(&_rxUartQueue, &c);
            count--;
        }
    }

    return status;
}

CmdStatus Uart::init(uint8_t const *cmd) {
    uint32_t baudrate = convertBytesToUInt32(&cmd[2]);
    uart_init(_uart, baudrate);
    gpio_set_function(Pin::ID::GP0_UART0_TX, GPIO_FUNC_UART);
    gpio_set_function(Pin::ID::GP1_UART0_RX, GPIO_FUNC_UART);
    setInterfaceState(InterfaceState::INTIALIZED);
    return CmdStatus::OK;
}

CmdStatus Uart::deInit() {
    /*TODO*/
    setInterfaceState(InterfaceState::NOT_INITIALIZED);
    return CmdStatus::OK;
}

// | UART_READ_FROM_UART | CmdStatus::OK | NB_BYTES[1] | PAYLOAD
CmdStatus Uart::read(const uint8_t *report, uint8_t *response){
    (void)report;
    uint8_t size = static_cast<uint8_t>(std::min(queue_get_level(&_rxUartQueue), HID_RESPONSE_SIZE - 3u ));
    response[2] = size;
    uint8_t index = 3;
    while(index <(3+size)) {
        uint8_t c;
        queue_try_remove(&_rxUartQueue, &c);
        response[index++] = c;
    }

    return CmdStatus::OK;
}

// | UART0_WRITE | NB_BYTES[1] | PAYLOAD |=> First | UART_WRITE | CmdStatus::OK |
CmdStatus Uart::write(const uint8_t *cmd){
    uint8_t payload = cmd[1];
    for(uint8_t it=0; it < payload; it++) {
        uart_putc_raw(_uart, cmd[2+it]);
    }
    return CmdStatus::OK;
}
