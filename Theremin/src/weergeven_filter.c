#include <avr/io.h>
#include "twi.h"
#include "weergeven_filter.h"

#define I2C_ADDR 0x21

// 7-segment bitpatronen voor common anode display
// Format: dp, g, f, e, d, c, b, a
const uint8_t segment_map[16] = {
    0b11000000, // 0
    0b11111001, // 1
    0b10100100, // 2
    0b10110000, // 3
    0b10011001, // 4
    0b10010010, // 5
    0b10000010, // 6
    0b11111000, // 7
    0b10000000, // 8
    0b10010000, // 9
    0b10001000, // A (10)
    0b10000011, // b (11)
    0b11000110, // C (12)
    0b10100001, // d (13)
    0b10000110, // E (14)
    0b10001110  // F (15)
};

void display_digit(uint8_t digit) {
    if (digit > 15) return;
    
    TWI_MT_Start();
    TWI_Transmit_SLAW(I2C_ADDR);
    TWI_Transmit_Byte(segment_map[digit]);
    TWI_Stop();
}

void display_filter_size(uint8_t size) {
    if (size > 15) size = 15;
    display_digit(size);
}