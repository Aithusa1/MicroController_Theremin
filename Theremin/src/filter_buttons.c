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
    DDRD &= ~(BUTTON_UP_MASK | BUTTON_DOWN_MASK);
    PORTD |= (BUTTON_UP_MASK | BUTTON_DOWN_MASK);
    filter_size_ptr = filter_get_size_ptr();
}

void filter_buttons_update(void) {
    static uint8_t last_state = 0xFF;
    uint8_t current_state = PIND & (BUTTON_UP_MASK | BUTTON_DOWN_MASK);
    
    if (current_state == last_state) {
        return;
    }
    
    _delay_ms(50);
    
    current_state = PIND & (BUTTON_UP_MASK | BUTTON_DOWN_MASK);
    
    if ((current_state & BUTTON_UP_MASK) == 0 && (last_state & BUTTON_UP_MASK)) {
        uint8_t current_size = *filter_size_ptr;
        if (current_size < 15) {
            uint8_t new_size = current_size + 2;
            if (new_size > 15) new_size = 15;
            filter_set_size(new_size);
            display_filter_size(new_size);
        }
    }
    
    if ((current_state & BUTTON_DOWN_MASK) == 0 && (last_state & BUTTON_DOWN_MASK)) {
        uint8_t current_size = *filter_size_ptr;
        if (current_size > 1) {
            uint8_t new_size = current_size - 2;
            if (new_size < 1) new_size = 1;
            filter_set_size(new_size);
            display_filter_size(new_size);
        }
    }
    
    last_state = current_state;
}

uint8_t filter_buttons_get_size(void) {
    return *filter_size_ptr;
}