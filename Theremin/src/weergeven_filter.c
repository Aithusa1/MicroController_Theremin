#include <avr/io.h>
#include "twi.h"
#include "weergeven_filter.h"

#define I2C_ADDR 0x21

// 7-segment bitpatronen voor common anode display
// Format: P7-P0 = DP, A, B, C, D, E, F, G
// Gebruikt kleine hex letters (a-f) voor filterwaarden 10-15
const uint8_t segment_map[16] = {
    0xc0, // 0 - A,B,C,D,E,F
    0xf9, // 1 - B,C  
    0xa4, // 2 - A,B,D,E,G
    0xb0, // 3 - A,B,C,D,G
    0x99, // 4 - B,C,F,G
    0x92, // 5 - A,C,D,F,G
    0x82, // 6 - A,C,D,E,F,G
    0xf8, // 7 - A,B,C
    0x80, // 8 - A,B,C,D,E,F,G
    0x90, // 9 - A,B,C,D,F,G
    0x88, // a (10) - A,B,C,E,F,G
    0x83, // b (11) - C,D,E,F,G
    0xc6, // c (12) - A,D,E,F
    0xa1, // d (13) - B,C,D,E,G
    0x86, // e (14) - A,D,E,F,G
    0x8e  // f (15) - A,E,F,G
};

// Toon filtergrootte op 7-segment display
void display_filter_size(uint8_t size) {
    // Begrens tot maximaal 15 (0xf)
    if (size > 15) size = 15;
    
    // I2C communicatie met port expander
    TWI_MT_Start();
    TWI_Transmit_SLAW(I2C_ADDR);
    // Stuur bitpatroon voor gevraagde waarde
    TWI_Transmit_Byte(segment_map[size]);
    TWI_Stop();
}