#include <avr/io.h>
#include <util/delay.h>
#include "twi.h"
#include "weergeven_filter.h"

#define BUTTON_UP_PIN   PD4
#define BUTTON_DOWN_PIN PD5
#define BUTTON_UP_MASK  (1 << BUTTON_UP_PIN)
#define BUTTON_DOWN_MASK (1 << BUTTON_DOWN_PIN)

uint8_t filter_size = 1; // Start met filter grootte 1

void init_buttons(void) {
    // PD4 en PD5 als input met interne pull-up
    DDRD &= ~(BUTTON_UP_MASK | BUTTON_DOWN_MASK);
    PORTD |= (BUTTON_UP_MASK | BUTTON_DOWN_MASK);
}

uint8_t read_buttons(void) {
    uint8_t button_state = 0;
    
    // Lees knop status (actief laag vanwege pull-up)
    if (!(PIND & BUTTON_UP_MASK)) {
        button_state |= 0x01; // Knop omhoog ingedrukt
    }
    if (!(PIND & BUTTON_DOWN_MASK)) {
        button_state |= 0x02; // Knop omlaag ingedrukt
    }
    
    return button_state;
}

void update_filter_size(uint8_t buttons) {
    static uint8_t last_buttons = 0;
    
    // Detect rising edge (knop losgelaten) voor debounce
    uint8_t pressed = (last_buttons & ~buttons);
    last_buttons = buttons;
    
    if (pressed & 0x01) { // Knop omhoog losgelaten
        if (filter_size < 15) {
            filter_size++;
        }
    }
    if (pressed & 0x02) { // Knop omlaag losgelaten
        if (filter_size > 1) {
            filter_size--;
        }
    }
}

int main(void) {
    // Initialisatie
    TWI_Init();
    init_buttons();
    _delay_ms(100);
    
    // Toon initiële filter grootte
    display_filter_size(filter_size);

    while (1) {
        // Lees knoppen
        uint8_t buttons = read_buttons();
        
        // Update filter grootte als nodig
        update_filter_size(buttons);
        
        // Toon huidige filter grootte
        display_filter_size(filter_size);
        
        _delay_ms(100); // Korte vertraging voor debounce
    }
}