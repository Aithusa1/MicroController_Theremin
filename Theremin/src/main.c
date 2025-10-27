#include <avr/io.h>
#include <util/delay.h>
#include "twi.h"
#include "weergeven_filter.h"

int main(void) {
    TWI_Init();
    _delay_ms(100);

    while (1) {
        display_digit(0);
        _delay_ms(1000);
    }
}