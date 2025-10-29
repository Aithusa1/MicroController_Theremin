#include <avr/io.h>
#include "twi.h"
#include "weergeven_filter.h"

#define I2C_ADDR 0x21

// 7-segment bitpatronen voor common anode display volgens gameshield
// Format: P7-P0 = DP, A, B, C, D, E, F, G
const uint8_t segment_map[16] = {
    0xC0, // 0 - A,B,C,D,E,F
    0xF9, // 1 - B,C  
    0xA4, // 2 - A,B,D,E,G
    0xB0, // 3 - A,B,C,D,G
    0x99, // 4 - B,C,F,G
    0x92, // 5 - A,C,D,F,G
    0x82, // 6 - A,C,D,E,F,G
    0xF8, // 7 - A,B,C
    0x80, // 8 - A,B,C,D,E,F,G
    0x90, // 9 - A,B,C,D,F,G
    0x88, // A (10) - A,B,C,E,F,G
    0x83, // b (11) - C,D,E,F,G
    0xC6, // C (12) - A,D,E,F
    0xA1, // d (13) - B,C,D,E,G
    0x86, // E (14) - A,D,E,F,G
    0x8E  // F (15) - A,E,F,G
};

void display_filter_size(uint8_t size) {
    if (size > 15) size = 15;
    
    TWI_MT_Start();
    TWI_Transmit_SLAW(I2C_ADDR);
    TWI_Transmit_Byte(segment_map[size]);
    TWI_Stop();
}