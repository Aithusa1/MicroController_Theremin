#include <avr/io.h>
#include <util/delay.h>
#include "twi.h"

#define LCD_ADDR 0x27

// Simpele LCD functies
void lcd_command(uint8_t cmd) {
    uint8_t high = (cmd & 0xF0) | 0x08;
    uint8_t low = ((cmd << 4) & 0xF0) | 0x08;
    
    TWI_MT_Start();
    TWI_Transmit_SLAW(LCD_ADDR);
    TWI_Transmit_Byte(high | 0x04);
    TWI_Transmit_Byte(high);
    TWI_Transmit_Byte(low | 0x04);
    TWI_Transmit_Byte(low);
    TWI_Stop();
    _delay_us(100);
}

void lcd_data(uint8_t data) {
    uint8_t high = (data & 0xF0) | 0x09;
    uint8_t low = ((data << 4) & 0xF0) | 0x09;
    
    TWI_MT_Start();
    TWI_Transmit_SLAW(LCD_ADDR);
    TWI_Transmit_Byte(high | 0x04);
    TWI_Transmit_Byte(high);
    TWI_Transmit_Byte(low | 0x04);
    TWI_Transmit_Byte(low);
    TWI_Stop();
    _delay_us(100);
}

void lcd_print(char *str) {
    while(*str) {
        lcd_data(*str++);
    }
}

void lcd_init() {
    _delay_ms(50);
    
    // Initialisatie
    lcd_command(0x33);
    _delay_ms(5);
    lcd_command(0x32);
    _delay_ms(5);
    lcd_command(0x28); // 2 lines, 5x8 font
    lcd_command(0x0C); // Display on, cursor off
    lcd_command(0x06); // Entry mode
    lcd_command(0x01); // Clear display
    _delay_ms(2);
}

int main(void) {
    TWI_Init();
    _delay_ms(1000);
    
    lcd_init();
    
    // Print "Hallo"
    lcd_print("Hallo");
    
    while(1) {
        _delay_ms(1000);
    }
}