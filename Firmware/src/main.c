#include "main.h"

int main(void) {
    // Serial begin
    uartBegin(BAUDRATE);
    setCommandDelimiter('\r');
    setCommandHandler(onCommandReceived);

    // Enable global interrupts and sleep mode
    set_sleep_mode(SLEEP_MODE_IDLE);
    sei();

    while (1) {
        

        // Enter sleep mode
        sleep_mode();
    }
    return 0;
}

void onCommandReceived(uint8_t* data, uint8_t length) {

}
