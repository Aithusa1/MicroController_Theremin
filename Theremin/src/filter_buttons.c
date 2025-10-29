#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "filter_buttons.h"
#include "filter_toonhoogte.h"
#include "weergeven_filter.h"
#include "weergeven_afstand_hoogte.h"

#define BUTTON_UP_PIN   PD4
#define BUTTON_DOWN_PIN PD5
#define BUTTON_UP_MASK  (1 << BUTTON_UP_PIN)
#define BUTTON_DOWN_MASK (1 << BUTTON_DOWN_PIN)

static uint8_t* filter_size_ptr;

void filter_buttons_init(void) {
    // PD4 en PD5 als input met interne pull-up
    DDRD &= ~(BUTTON_UP_MASK | BUTTON_DOWN_MASK);
    PORTD |= (BUTTON_UP_MASK | BUTTON_DOWN_MASK);
    
    // Krijg pointer naar filter_size
    filter_size_ptr = filter_get_size_ptr();
}

void filter_buttons_update(void) {
    static uint8_t last_state = 0xFF;
    uint8_t current_state = PIND & (BUTTON_UP_MASK | BUTTON_DOWN_MASK);
    
    // Alleen verwerken als er iets veranderd is
    if (current_state == last_state) {
        return;
    }
    
    _delay_ms(50); // Debounce
    
    // Opnieuw lezen na debounce
    current_state = PIND & (BUTTON_UP_MASK | BUTTON_DOWN_MASK);
    
    // Knop omhoog (PD4) - filter groter (alleen oneven: 1,3,5,7,9,11,13,15)
    if ((current_state & BUTTON_UP_MASK) == 0 && (last_state & BUTTON_UP_MASK)) {
        uint8_t current_size = *filter_size_ptr;
        if (current_size < 15) {
            // Ga naar volgende oneven getal
            uint8_t new_size = current_size + 2;
            if (new_size > 15) new_size = 15;
            filter_set_size(new_size);
            display_filter_size(new_size);
            
            // Debug
            char debug[16];
            snprintf(debug, sizeof(debug), "UP:%2d", new_size);
            lcd_command(0xCA);
            lcd_print(debug);
        } else {
            // Al op maximum - blijf op 15
            lcd_command(0xCA);
            lcd_print("MAX15");
        }
    }
    
    // Knop omlaag (PD5) - filter kleiner (alleen oneven: 1,3,5,7,9,11,13,15)
    if ((current_state & BUTTON_DOWN_MASK) == 0 && (last_state & BUTTON_DOWN_MASK)) {
        uint8_t current_size = *filter_size_ptr;
        if (current_size > 1) {
            // Ga naar vorige oneven getal
            uint8_t new_size = current_size - 2;
            if (new_size < 1) new_size = 1;
            filter_set_size(new_size);
            display_filter_size(new_size);
            
            // Debug
            char debug[16];
            snprintf(debug, sizeof(debug), "DN:%2d", new_size);
            lcd_command(0xCA);
            lcd_print(debug);
        } else {
            // Al op minimum - blijf op 1
            lcd_command(0xCA);
            lcd_print("MIN 1");
        }
    }
    
    last_state = current_state;
}

uint8_t filter_buttons_get_size(void) {
    return *filter_size_ptr;
}