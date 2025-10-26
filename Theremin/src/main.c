#include <avr/io.h>
#include <util/delay.h>
#include "twi.h"

#define I2C_ADDR 0x27  // Gameshield gebruikt meestal 0x27

// 7-segment bitpatronen (common cathode via PCF8574)
const uint8_t segment_map[10] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111  // 9
};

void display_digit(uint8_t digit)
{
    if (digit > 9) return;
    
    // Gebruik de TWI library functies
    TWI_MT_Start();                    // Start I2C communicatie
    TWI_Transmit_SLAW(I2C_ADDR);       // Stuur adres + write
    TWI_Transmit_Byte(segment_map[digit]); // Stuur segment data
    TWI_Stop();                        // Stop I2C communicatie
}

int main(void)
{
    TWI_Init();  // Initialiseer TWI/I2C via library
    _delay_ms(100);

    while (1)
    {
        // Toon cijfers 0-9, elk 1 seconde
        for (uint8_t i = 0; i < 10; i++)
        {
            display_digit(i);
            _delay_ms(1000);
        }
    }
}