
// #include <stdio.h>
// #include <avr/io.h>
// #include <util/delay.h>
// #include "newMath.h"



// int main(){
//     DDRB |= (1 << DDB5); // Set pin 13 as output

//     while(1){
//         PORTB ^= (1 << PORTB5); // Toggle pin 13
//         unsigned int delay = multiply(2, 1000); // Call multiply function
//         _delay_ms(delay); // Wait for 1 second
//     }
//     return -1;
// }



#include <stdio.h>
int main()
{
    int waarde;
    printf("%d\n", waarde);
    return 0;
}










// #define F_CPU 16000000UL
// #include <avr/io.h>
// #include <util/delay.h>
// #include "twi.h"

// // ================= LCD I2C DEFINITIES =================
// #define LCD_I2C_ADDR 0x27  // typ. 0x27 of 0x3F
// #define LCD_BACKLIGHT 0x08
// #define LCD_ENABLE    0x04
// #define LCD_COMMAND   0
// #define LCD_DATA      1

// // ================= LCD I2C FUNCTIES =================
// void lcd_i2c_write(uint8_t data)
// {
//     TWI_MT_Start();
//     TWI_Transmit_SLAW(LCD_I2C_ADDR);
//     TWI_Transmit_Byte(data);
//     TWI_Stop();
//     _delay_us(50);
// }

// void lcd_i2c_send(uint8_t value, uint8_t mode)
// {
//     uint8_t highnib = value & 0xF0;
//     uint8_t lownib = (value << 4) & 0xF0;
//     uint8_t data;

//     for (int i = 0; i < 2; i++) {
//         data = (i == 0 ? highnib : lownib);
//         lcd_i2c_write(data | mode | LCD_BACKLIGHT | LCD_ENABLE);
//         lcd_i2c_write(data | mode | LCD_BACKLIGHT);
//     }
// }

// void lcd_init_i2c(void)
// {
//     TWI_Init();
//     _delay_ms(50);

//     lcd_i2c_send(0x33, LCD_COMMAND);
//     lcd_i2c_send(0x32, LCD_COMMAND);
//     lcd_i2c_send(0x28, LCD_COMMAND); // 4-bit, 2 lijnen
//     lcd_i2c_send(0x0C, LCD_COMMAND); // display aan, cursor uit
//     lcd_i2c_send(0x06, LCD_COMMAND); // auto increment
//     lcd_i2c_send(0x01, LCD_COMMAND); // clear
//     _delay_ms(5);
// }

// void lcd_clear_i2c(void)
// {
//     lcd_i2c_send(0x01, LCD_COMMAND);
//     _delay_ms(2);
// }

// void lcd_set_cursor_i2c(uint8_t col, uint8_t row)
// {
//     uint8_t row_offsets[] = {0x00, 0x40};
//     lcd_i2c_send(0x80 | (col + row_offsets[row]), LCD_COMMAND);
// }

// void lcd_print_i2c(const char *str)
// {
//     while (*str) {
//         lcd_i2c_send(*str++, LCD_DATA);
//     }
// }

// // ================= MAIN =================
// int main(void)
// {
//     lcd_init_i2c();
//     lcd_clear_i2c();
//     lcd_set_cursor_i2c(0,0);
//     lcd_print_i2c("Hello");  // Toon gewoon "Hello"

//     while(1)
//     {
//         // oneindige lus, doet verder niks
//     }
// }
