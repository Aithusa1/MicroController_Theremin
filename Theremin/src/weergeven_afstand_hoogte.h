#ifndef WEERGEVEN_AFSTAND_HOOGTE_H
#define WEERGEVEN_AFSTAND_HOOGTE_H

#include <stdint.h>

#define LCD_ADDR 0x27

// Functie declaraties
void lcd_init(void);
void lcd_command(uint8_t cmd);
void lcd_data(uint8_t data);
void lcd_print(char *str);

#endif