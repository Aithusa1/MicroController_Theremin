#include <avr/io.h>
#include <util/delay.h>
#include "twi.h"

#define I2C_ADDR 0x21

// 7-segment bitpatronen - aangepast gebaseerd op wat werkt
const uint8_t segment_map[10] = {
    0b11000000, // cijfer 0
    0b10110000, // cijfer 3
    0b10011001, // cijfer 4
};

void display_digit(uint8_t digit) {
    if (digit > 9) return;
    
    TWI_MT_Start();
    TWI_Transmit_SLAW(I2C_ADDR);
    TWI_Transmit_Byte(segment_map[digit]);
    TWI_Stop();
}

int main(void) {
    TWI_Init();
    _delay_ms(100);

    while (1) {
        // Toon alleen cijfers 0-9
        
            display_digit(0);
            _delay_ms(1000);
        
    }
}