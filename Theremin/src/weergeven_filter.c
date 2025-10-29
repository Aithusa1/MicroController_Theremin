#include <avr/io.h>
#include "twi.h"
#include "weergeven_filter.h"

#define I2C_ADDR 0x21

const uint8_t segment_map[16] = {
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};

void display_filter_size(uint8_t size) {
    if (size > 15) size = 15;
    
    TWI_MT_Start();
    TWI_Transmit_SLAW(I2C_ADDR);
    TWI_Transmit_Byte(segment_map[size]);
    TWI_Stop();
}