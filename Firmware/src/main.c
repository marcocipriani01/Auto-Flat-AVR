#include "main.h"

int main(void) {
    // Serial begin
    uartBegin(BAUDRATE);

    // Enable global interrupts and sleep mode
    set_sleep_mode(SLEEP_MODE_IDLE);
    sei();

    while (1) {


        // Enter sleep mode
        sleep_mode();
    }
    return 0;
}
